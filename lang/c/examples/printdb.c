/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to you under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.  See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <avro.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define QUICKSTOP_CODEC  "deflate"
//#define QUICKSTOP_CODEC  "qat"


/* A simple schema for our tutorial */
const char  PERSON_SCHEMA[] =
"{\"type\":\"record\",\
  \"name\":\"Person\",\
  \"fields\":[\
     {\"name\": \"ID\", \"type\": \"long\"},\
     {\"name\": \"First\", \"type\": \"string\"},\
     {\"name\": \"Last\", \"type\": \"string\"},\
     {\"name\": \"Phone\", \"type\": \"string\"},\
     {\"name\": \"Age\", \"type\": \"int\"}]}";

/* Parse schema into a schema data structure */
void init_schema(avro_schema_t *schema)
{
	if (avro_schema_from_json_literal(PERSON_SCHEMA, schema)) {
		fprintf(stderr, "Unable to parse person schema\n");
		exit(EXIT_FAILURE);
	}
}

int print_person(avro_file_reader_t db, avro_schema_t reader_schema)
{

	avro_value_iface_t  *person_class =
	    avro_generic_class_from_schema(reader_schema);

	avro_value_t person;
	avro_generic_value_new(person_class, &person);

	int rval;

	rval = avro_file_reader_read_value(db, &person);
	if (rval == 0) {
		int64_t id;
		int32_t age;
		int32_t *p;
		size_t size;
		avro_value_t id_value;
		avro_value_t first_value;
		avro_value_t last_value;
		avro_value_t age_value;
		avro_value_t phone_value;

		if (avro_value_get_by_name(&person, "ID", &id_value, NULL) == 0) {
			avro_value_get_long(&id_value, &id);
			fprintf(stdout, "%"PRId64" | ", id);
		}
		if (avro_value_get_by_name(&person, "First", &first_value, NULL) == 0) {
			avro_value_get_string(&first_value, &p, &size);
			fprintf(stdout, "%15s | ", p);
		}
		if (avro_value_get_by_name(&person, "Last", &last_value, NULL) == 0) {
			avro_value_get_string(&last_value, &p, &size);
			fprintf(stdout, "%15s | ", p);
		}
		if (avro_value_get_by_name(&person, "Phone", &phone_value, NULL) == 0) {
			avro_value_get_string(&phone_value, &p, &size);
			fprintf(stdout, "%15s | ", p);
		}
		if (avro_value_get_by_name(&person, "Age", &age_value, NULL) == 0) {
			avro_value_get_int(&age_value, &age);
			fprintf(stdout, "%"PRId32" | ", age);
		}
		fprintf(stdout, "\n");

		/* We no longer need this memory */
		avro_value_decref(&person);
		avro_value_iface_decref(person_class);
	}
	return rval;
}

int main(void)
{
	int rval;
	avro_file_reader_t reader;
	const char *dbname = "quickstop.db";
    avro_schema_t person_schema;

	/* Initialize the schema structure from JSON */
	init_schema(&person_schema);

	fprintf(stdout, "\nNow let's read all the records back out\n");

	/* Read all the records and print them */
	if (avro_file_reader(dbname, &reader)) {
		fprintf(stderr, "Error opening file: %s\n", avro_strerror());
		exit(EXIT_FAILURE);
	}
    clock_t t;
    t = clock();
    int id = 1000000;
	for (int i = 0; i < id; i++) {
		if (print_person(reader, person_schema)) {
			fprintf(stderr, "Error printing person\nMessage: %s\n", avro_strerror());
			exit(EXIT_FAILURE);
		}
	}
    t = clock() - t;
    double time_taken = ((double)t)*1000/CLOCKS_PER_SEC;
    fprintf(stderr, "adding persons takes %f ms.\n", time_taken);
	avro_file_reader_close(reader);

#if 0
	avro_schema_t projection_schema, first_name_schema, phone_schema;
	/* You can also use projection, to only decode only the data you are
	   interested in.  This is particularly useful when you have 
	   huge data sets and you'll only interest in particular fields
	   e.g. your contacts First name and phone number */
	projection_schema = avro_schema_record("Person", NULL);
	first_name_schema = avro_schema_string();
	phone_schema = avro_schema_string();
	avro_schema_record_field_append(projection_schema, "First",
					first_name_schema);
	avro_schema_record_field_append(projection_schema, "Phone",
					phone_schema);

	/* Read only the record you're interested in */
	fprintf(stdout,
		"\n\nUse projection to print only the First name and phone numbers\n");
	if (avro_file_reader(dbname, &reader)) {
		fprintf(stderr, "Error opening file: %s\n", avro_strerror());
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < id; i++) {
		if (print_person(reader, projection_schema)) {
			fprintf(stderr, "Error printing person: %s\n",
				avro_strerror());
			exit(EXIT_FAILURE);
		}
	}
	avro_file_reader_close(reader);
	avro_schema_decref(first_name_schema);
	avro_schema_decref(phone_schema);
	avro_schema_decref(projection_schema);

	/* We don't need this schema anymore */
	avro_schema_decref(person_schema);
#endif
	return 0;
}

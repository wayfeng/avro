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
#include <unistd.h>
#include <time.h>

//#define QUICKSTOP_CODEC "lzma"
//#define QUICKSTOP_CODEC "snappy"
//#define QUICKSTOP_CODEC  "deflate"
#define QUICKSTOP_CODEC  "qat"

int64_t id = 0;

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

/* Create a value to match the person schema and save it */
void
add_person(avro_file_writer_t db, avro_schema_t schema,
       const char *first, const char *last, const char *phone, int32_t age)
{
	avro_value_t  person;
    avro_value_iface_t *person_class = avro_generic_class_from_schema(schema);
	avro_generic_value_new(person_class, &person);

	avro_value_t id_value;
	avro_value_t first_value;
	avro_value_t last_value;
	avro_value_t age_value;
	avro_value_t phone_value;

	if (avro_value_get_by_name(&person, "ID", &id_value, NULL) == 0) {
		avro_value_set_long(&id_value, ++id);
	}
	if (avro_value_get_by_name(&person, "First", &first_value, NULL) == 0) {
		avro_value_set_string(&first_value, first);
	}
	if (avro_value_get_by_name(&person, "Last", &last_value, NULL) == 0) {
		avro_value_set_string(&last_value, last);
	}
	if (avro_value_get_by_name(&person, "Age", &age_value, NULL) == 0) {
		avro_value_set_int(&age_value, age);
	}
	if (avro_value_get_by_name(&person, "Phone", &phone_value, NULL) == 0) {
		avro_value_set_string(&phone_value, phone);
	}

	if (avro_file_writer_append_value(db, &person)) {
		fprintf(stderr,
			"Unable to write Person value to memory buffer\nMessage: %s\n", avro_strerror());
		exit(EXIT_FAILURE);
	}

	/* Decrement all our references to prevent memory from leaking */
	avro_value_decref(&person);
	avro_value_iface_decref(person_class);
}

int main(int argc, char* argv[])
{
	int rval;
	avro_file_writer_t db;
	int64_t i;
	const char *dbname = "quickstop.db";
	char number[15] = {0};

	/* Initialize the schema structure from JSON */
    avro_schema_t person_schema;
	init_schema(&person_schema);

	/* Delete the database if it exists */
	remove(dbname);
	/* Create a new database */
	rval = avro_file_writer_create_with_codec
	    (dbname, person_schema, &db, QUICKSTOP_CODEC, 0);
	if (rval) {
		fprintf(stderr, "There was an error creating %s\n", dbname);
		fprintf(stderr, " error message: %s\n", avro_strerror());
		exit(EXIT_FAILURE);
	}

    clock_t t;
    t = clock();
	/* Add lots of people to the database */
	for (i = 0; i < 1000000; i++)
	{
		sprintf(number, "(%d)", (int)i);
		add_person(db, person_schema, "Dante", "Hicks", number, 32);
		add_person(db, person_schema, "Randal", "Graves", "(555) 123-5678", 30);
		add_person(db, person_schema, "Veronica", "Loughran", "(555) 123-0987", 28);
		add_person(db, person_schema, "Caitlin", "Bree", "(555) 123-2323", 27);
		add_person(db, person_schema, "Bob", "Silent", "(555) 123-6422", 29);
		add_person(db, person_schema, "Jay", "???", number, 26);
	}
    t = clock() - t;
    double time_taken = ((double)t)*1000/CLOCKS_PER_SEC;
    fprintf(stderr, "adding persons takes %f ms.\n", time_taken);

	/* Close the block and open a new one */
	avro_file_writer_flush(db);
	//add_person(db, "Super", "Man", "123456", 31);

	avro_file_writer_close(db);
	avro_schema_decref(person_schema);

	return 0;
}

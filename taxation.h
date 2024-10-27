#ifndef __TAXATION_H__
#define __TAXATION_H__
#include "csv.h"
#include "date.h"

// Ex 1: Define data types here...

///////////////////////////
#define MAX_TENANTS 50
#define MAX_PROPERTIES 80
#define MAX_LANDLORDS 10
#define MAX_NAME 15
#define MAX_CADASTRAL_REF 7
#define MAX_STREET 25
#define MAX_PERSON_ID 9
#define AMOUNT_NO_RENT 150.0

typedef struct _tNotDeclared {
    char cadastral_ref[MAX_CADASTRAL_REF];
    char landlord_id[MAX_PERSON_ID];
    int year;
} tNotDeclared;

typedef struct _tNotDeclaredData {
    tNotDeclared elems[MAX_PROPERTIES];
    int count;
} tNotDeclaredData;

typedef struct _tTenant {
    tDate start_date;
    tDate end_date;
    char tenant_id[MAX_PERSON_ID + 1];
    char name[MAX_NAME];
    float rent;
    int age;
    char cadastral_ref[MAX_CADASTRAL_REF + 1];
} tTenant;

typedef struct _tAddress {
    char street[MAX_STREET];
    int number;
} tAddress;

typedef struct _tProperty {
    char cadastral_ref[MAX_CADASTRAL_REF + 1];
    tAddress address;
    char landlord_id[MAX_PERSON_ID + 1];
} tProperty;

typedef struct _tProperties {
    tProperty elems[MAX_PROPERTIES];
    int count;
} tProperties;

typedef struct _tLandlord {
    char name[MAX_NAME];
    char id[MAX_PERSON_ID + 1];
    float tax;
    tProperties properties;
} tLandlord;

typedef struct _tLandlords {
    tLandlord* elems;
    int count;
} tLandlords;

typedef struct _tLandlordsE {
    tLandlord elems[MAX_LANDLORDS];
    int count;
} tLandlordsE;

//////////////////////////////////
// Available methods
//////////////////////////////////
// Initialize the properties data
void properties_init(tProperties* data);

// Get the number of properties
int properties_len(tLandlord data);

// Initialize the landlords data
void landlords_init(tLandlords* data);

// Get the number of landlords
int landlords_len(tLandlords data);

/////////////////////////////////////
// Ex 2: Requested methods ....
/////////////////////////////////////

// Parse input from CSVEntry
void landlord_parse(tLandlord* data, tCSVEntry entry);

// Add a new tenant
void landlords_add(tLandlords* data, tLandlord tenant);

//Remove a landlord
void landlords_del(tLandlords* data, char* id);

// Remove all elements
void landlords_free(tLandlords* data);

// Copy the data from the source to destination
void landlords_cpy(tLandlords* destination, tLandlords source);

// Parse input from CSVEntry
void property_parse(tProperty* data, tCSVEntry entry);

// Add a new property
void landlord_add_property(tLandlords* data, tProperty property);

// Parse input from CSVEntry
void tenant_parse(tTenant* data, tCSVEntry entry);

// Calculate and update taxation given tenant
void landlords_process_tenant(tLandlords* data, tTenant tenant);

// Get landlord data in a string
void landlord_get(tLandlords data, int index, char* buffer);

// Get a property data in a string
void property_get(tLandlord data, int index, char* buffer);

// Returns true if field tax of expected[index] is greater than the one in declarant[index]
bool mismatch_tax_declaration(tLandlords expected, tLandlords declarant, int index);
// Count properties not declared in a given year by their current owners only. Ignore if the owner was different at the time.
int countNotDeclared(tLandlordsE landlords, int year, tNotDeclaredData notDeclared);

/////////////////////////////////////
// Aux methods
/////////////////////////////////////

// [AUX METHOD] Return the position of a property entry with provided information. -1 if it does not exist
int properties_find(tProperties data, const char* cadastral_ref);

// [AUX METHODS] Copy the data from the source to destination
void property_cpy(tProperty* destination, tProperty source);

// [AUX METHODS] returns the position of a landlord that has a property with that cadastral ref
int landlords_find_by_cadastral_ref(tLandlords data, const char* cadastral_ref);

// [AUX METHOD] Return the position of a landl entry with that landlord id. -1 if it does not exist
int landlords_find(tLandlords data, const char* landlord_id);

// [AUX METHODS] Copy the data from the source to destination
void landlord_cpy(tLandlord* destination, tLandlord source);

////////////////////////////////////////////
#endif

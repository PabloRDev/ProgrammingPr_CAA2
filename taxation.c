#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "taxation.h"

//////////////////////////////////
// Available methods
//////////////////////////////////

// Parse a tDateTime from string information
void date_parse(tDate *date, const char *s_date) {
    // Check output data
    assert(date != NULL);

    // Check input date
    assert(s_date != NULL);
    assert(strlen(s_date) == 10);

    // Parse the input date
    sscanf(s_date, "%d/%d/%d", &(date->day), &(date->month), &(date->year));
}

// Initialize the properties
void properties_init(tProperties *data) {
    /////////////
    // Set the initial number of elements to zero.
    data->count = 0;
    /////////////
}

// Get the number of properties
int properties_len(tLandlord data) {
    //////////////
    // Return the number of elements
    return data.properties.count;
    //////////////
}

// Initialize the landlords:
// Given a structure of type tLandlords, it initializes it correctly
//obtaining an empty structure.
void landlords_init(tLandlords *data) {
    // Ex. 2a
    /////////////
    // Set the initial number of elements to zero.
    data->elems = NULL;
    data->count = 0;
    /////////////
}

// Get the number of landlords
int landlords_len(tLandlords data) {
    //////////////
    // Return the number of elements
    return data.count;
    //////////////
}


//////////////////////////////////////
// EX2: Implement your methods here....
//////////////////////////////////////

// Parse input from CSVEntry
void tenant_parse(tTenant *data, tCSVEntry entry) {
    // TODO
    char start_date[11];
    char end_date[11];

    // Check input data (Pre-conditions)
    assert(data != NULL);
    assert(csv_numFields(entry) == 7);

    // Get the date and time
    csv_getAsString(entry, 0, start_date, 11);
    date_parse(&(data->start_date), start_date);

    csv_getAsString(entry, 1, end_date, 11);
    date_parse(&(data->end_date), end_date);

    // Assign the tenant ID
    csv_getAsString(entry, 2, data->tenant_id, MAX_NAME + 1);

    // Assign the tenant name
    csv_getAsString(entry, 3, data->name, MAX_NAME + 1);

    data->rent = csv_getAsInteger(entry, 4);
    data->age = csv_getAsInteger(entry, 5);
    csv_getAsString(entry, 6, data->cadastral_ref, MAX_CADASTRAL_REF + 1);
}

////////////////////////////////////////
void landlords_process_tenant(tLandlords *data, tTenant tenant) {
    // TODO
    int idx;

    // Check input data (Pre-conditions)
    assert(data != NULL);

    // Check if an entry with this data already exists
    idx = landlords_find_by_cadastral_ref(*data, tenant.cadastral_ref);

    // If it does not exist, create a new entry
    if (idx >= 0) {
        int months_rented = tenant.end_date.month - tenant.start_date.month + 1;
        float tax_factor;
        float amount_to_add;
        if (tenant.age <= 35)
            tax_factor = 0.1;
        else
            tax_factor = 0.2;
        amount_to_add = months_rented * tenant.rent * tax_factor - AMOUNT_NO_RENT * months_rented;
        data->elems[idx].tax = data->elems[idx].tax + amount_to_add;
    }
}


// Get a property
void property_get(tLandlord data, int index, char *buffer) {
    // TODO
    assert(index < data.properties.count);
    sprintf(buffer, "%s;%s,%d;%s",
            data.properties.elems[index].cadastral_ref,
            data.properties.elems[index].address.street,
            data.properties.elems[index].address.number,
            data.properties.elems[index].landlord_id
    );
}

// Parse input from CSVEntry
void property_parse(tProperty *data, tCSVEntry entry) {
    // TODO
    // Check input data (Pre-conditions)
    assert(data != NULL);
    assert(csv_numFields(entry) == 4);

    // Get the date and time
    csv_getAsString(entry, 0, data->cadastral_ref, MAX_CADASTRAL_REF + 1);
    csv_getAsString(entry, 1, data->address.street, MAX_STREET + 1);
    data->address.number = csv_getAsInteger(entry, 2);
    csv_getAsString(entry, 3, data->landlord_id, MAX_PERSON_ID + 1);
}

////////////////////////////////////////

// Add a new property
void landlord_add_property(tLandlords *data, tProperty property) {
    // TODO
    int idx_landlord;

    // Check input data (Pre-conditions)
    assert(data != NULL);

    idx_landlord = landlords_find(*data, property.landlord_id);
    if (idx_landlord >= 0) {
        int idx_property = -1;

        // Check if an entry with this data already exists
        idx_property = properties_find(data->elems[idx_landlord].properties, property.cadastral_ref);

        // If it does not exist, create a new entry
        if (idx_property < 0) {
            int property_count = data->elems[idx_landlord].properties.count;
            assert(property_count < MAX_PROPERTIES);
            property_cpy(&(data->elems[idx_landlord].properties.elems[property_count]), property);
            data->elems[idx_landlord].properties.count++;
            data->elems[idx_landlord].tax = data->elems[idx_landlord].tax + AMOUNT_NO_RENT * 12;
        }
    }
}

// Get a landlord
void landlord_get(tLandlords data, int index, char *buffer) {
    // TODO
    assert(index < data.count);
    sprintf(buffer, "%s;%s,%.1f",
            data.elems[index].name,
            data.elems[index].id,
            data.elems[index].tax
    );
}

// Parse input from CSVEntry
void landlord_parse(tLandlord *data, tCSVEntry entry) {
    // TODO
    // Check input data (Pre-conditions)
    assert(data != NULL);
    assert(csv_numFields(entry) == 3);
    data->properties.count = 0;
    // Get the date and time
    csv_getAsString(entry, 0, data->name, MAX_NAME + 1);
    csv_getAsString(entry, 1, data->id, MAX_PERSON_ID + 1);
    data->tax = csv_getAsInteger(entry, 2);
}

////////////////////////////////////////

// Add a new landlord
void landlords_add(tLandlords *data, tLandlord landlord) {
    // Check input data (Pre-conditions)
    assert(data != NULL);

    // Check if an entry with this data already exists
    const int idx = landlords_find(*data, landlord.id);

    // If it does not exist, create a new entry
    if (idx < 0) {
        // Reallocate memory
        tLandlord *temp = realloc(data->elems, (data->count + 1) * sizeof(tLandlord));
        if (temp == NULL) {
            printf("\n Error: not enough free memory\n");
            return;
        }

        data->elems = temp;
        // Copy landlord to next landlord data
        landlord_cpy((&(data->elems[data->count])), landlord);
        data->count++;
    } else { printf("Landlord with ID '%s' already exists.\n", landlord.id); };
}

// Remove a landlord
void landlords_del(tLandlords *data, char *id) {
    // Ex. 2d
}

// returns true if field tax of expected[index] is greater than the one in declarant[index]
bool mismatch_tax_declaration(tLandlords expected, tLandlords declarant, int index) {
    // TODO
    return (expected.elems[index].tax > declarant.elems[index].tax);
}

// Copy the data from the source to destination:
// Copies a structure of type tLandlords into another structure
//of the same type, except for the amount to pay field, which is initialized to zero
//in all owners.
void landlords_cpy(tLandlords *destination, tLandlords source) {
    int i;

    destination->elems = malloc((source.count * sizeof(tLandlord)));
    if (destination->elems == NULL) {
            printf("\n Error: not enough free memory\n");
    }
    destination->count = source.count;

    for (i = 0; i < landlords_len(source); i++) {
        landlord_cpy(&(destination->elems[i]), source.elems[i]);
        // we want to copy all fields from source.elems[i] but want
        // to set tax to 0 in the expected landlords
        destination->elems[i].tax = 0.0;
    }
}

// [AUX METHOD] Return the position of a tenant entry with provided information. -1 if it does not exist
int landlords_find(tLandlords data, const char *id) {
    int i;
    int res = -1;

    i = 0;
    while ((i < data.count) && (res < 0)) {
        if ((strcmp(data.elems[i].id, id) == 0)) {
            res = i;
        } else {
            i++;
        }
    }

    return res;
}


// [AUX METHOD] Return the position of a tenant entry with provided information. -1 if it does not exist
int landlords_find_by_cadastral_ref(tLandlords data, const char *id) {
    int i, j;
    int res = -1;

    i = 0;
    while ((i < data.count) && (res < 0)) {
        j = 0;
        while ((j < data.elems[i].properties.count) && (res < 0)) {
            if ((strcmp(data.elems[i].properties.elems[j].cadastral_ref, id) == 0)) {
                res = i;
            }
            j++;
        }
        i++;
    }

    return res;
}


// [AUX METHODS] Copy the data from the source to destination
void landlord_cpy(tLandlord *destination, tLandlord source) {
    strcpy(destination->name, source.name);
    strcpy(destination->id, source.id);
    destination->tax = source.tax;

    // Properties are not copied. Initialize them to avoid errors.
    properties_init(&(destination->properties));
}

// [AUX METHOD] Return the position of a property entry with provided information. -1 if it does not exist
int properties_find(tProperties data, const char *cadastral_ref) {
    int i;
    int res = -1;

    i = 0;
    while ((i < data.count) && (res < 0)) {
        if ((strcmp(data.elems[i].cadastral_ref, cadastral_ref) == 0)) {
            res = i;
        } else {
            i++;
        }
    }
    return res;
}

// [AUX METHODS] Copy the data from the source to destination
void property_cpy(tProperty *destination, tProperty source) {
    strcpy(destination->cadastral_ref, source.cadastral_ref);
    strcpy(destination->address.street, source.address.street);
    destination->address.number = source.address.number;
    strcpy(destination->landlord_id, source.landlord_id);
}

// Remove all elements
void landlords_free(tLandlords *data) {
    // Ex. 2e
}

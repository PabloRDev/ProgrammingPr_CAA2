#include <stdio.h>
#include "csv.h"
#include "taxation.h"

void landlord_print(tLandlords data, int index, bool print_index) {
    char landlord_buffer[1024];
    char property_buffer[1024];
    int i;

    landlord_get(data, index, landlord_buffer);
    if (print_index) {
        printf("%d;%s\n", index, landlord_buffer);

        for(i = 0 ; i < properties_len(data.elems[index]) ; i++) {
            property_get(data.elems[index], i, property_buffer);
            printf("   %d;%s\n", i, property_buffer);
        }
    }    
    else
        printf("%s\n", landlord_buffer);
}

void landlords_print(tLandlords data) {
    int i;
    for(i = 0 ; i < landlords_len(data) ; i++) {
        landlord_print(data, i, true);
    }
}

int main(void)
{    
    tCSVData test_data;    
    int i;
    tLandlords declarant_landlords;
    tLandlord landlord;
    tLandlords expected_landlords;
    tProperty property;    
    tTenant tenant;

    const char* test_landlord_str = "Eric;42345671A;200.0\n" \
                                    "Sandra;52345671B;970.0\n" \
                                    "Karol;43345671C;4200.0\n" \
									"Lucas;33345671D;1800.0\n" \
                                    "Emma;45700291F;500.0\n";

    const char* test_property_str = "ABC1234;Balmes;25;42345671A\n" \
                                    "AAC2256;Villarroel;13;42345671A\n" \
                                    "ZBC1234;Bonanova;51;52345671B\n" \
                                    "QBC1234;Aribau;34;43345671C\n" \
                                    "TBC1234;Aribau;44;43345671C\n" \
                                    "YBC1234;Casanova;66;33345671D\n";

    const char* test_tenants_str = "01/01/2023;31/12/2023;12345678;Lucas;600.0;25;ABC1234\n" \
                                   "01/05/2023;31/12/2023;22345678;Juan;1200.0;52;AAC2256\n" \
                                   "01/01/2023;30/06/2023;32345678;Baltasar;700.0;28;ZBC1234\n" \
                                   "01/08/2023;31/12/2023;32345678;Daniel;800.0;34;ZBC1234\n" \
                                   "01/01/2023;31/12/2023;62345678;Jessica;1050.0;41;QBC1234\n" \
                                   "01/01/2023;25/11/2023;62345678;Anton;1050.0;41;TBC1234\n";

    csv_init(&test_data);
    csv_parse(&test_data, test_landlord_str, "LANDLORDS");
    
    // Initialize the landlords data
    landlords_init(&declarant_landlords);
    
    // Add data
    for(i=0; i < csv_numEntries(test_data); i++) {
        // Fills a landlord with all information but properties which it is set to zero
        landlord_parse(&landlord, csv_getEntry(test_data, i)[0]);        
        // Add new landlord to the list of landlords
        landlords_add(&declarant_landlords, landlord);
    }
	
	// Remove a landlord
	landlords_del(&declarant_landlords, "45700291F");
    
    // List current declarant landlords
    printf("Declarant landlords:\n");
    landlords_print(declarant_landlords);
    printf("\n");
    
    // Remove landlords data
    csv_free(&test_data);

    landlords_cpy(&expected_landlords, declarant_landlords);

    csv_init(&test_data);
    csv_parse(&test_data, test_property_str, "PROPERTIES");
    
    // Add properties to expected landlords
    for(i=0; i < csv_numEntries(test_data); i++) {
        // Initialize the position
        property_parse(&property, csv_getEntry(test_data, i)[0]);
        // Add new property to the list of tenants
        landlord_add_property(&expected_landlords, property);        
    }  
                                
    // Read the data in CSV format
    csv_init(&test_data);
    csv_parse(&test_data, test_tenants_str, "TENANTS");       

    // Add tenants to expected landlords
    for(i=0; i < csv_numEntries(test_data); i++) {
        // Initialize the position
        tenant_parse(&tenant, csv_getEntry(test_data, i)[0]);
        
        // Add new tenant to the list of tenants
        landlords_process_tenant(&expected_landlords, tenant);
    }
    
    // List current landlords
    printf("Expected landlords:\n");
    landlords_print(expected_landlords);

    printf("\n");
    
    // Remove tenants data
    csv_free(&test_data);

    for(i = 0 ; i < landlords_len(expected_landlords) ; i++) {
        if (mismatch_tax_declaration(expected_landlords, declarant_landlords, i)) {
            printf("Tax declaration mismatch: ");
            landlord_print(expected_landlords, i, false);
        }
    }

	landlords_free(&declarant_landlords);
	landlords_free(&expected_landlords);
    printf("\n\nPress key to end...\n");
    getchar();
	return 0;
}


#include "csv_test.h"
#include "ok_csv.h"
#include "ok_mo.h" // for UTF-8 code
#include "test_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int csv_test(const char *path, bool verbose) {
    (void)verbose;

    char *test1_file = get_full_path(path, "test1", "csv");

    FILE *file = fopen(test1_file, "rb");
    ok_csv *csv = ok_csv_read(file);
    fclose(file);
    free(test1_file);

    if (!csv) {
        printf("Failure: ok_csv is NULL\n");
        return 1;
    }
    if (csv->num_records == 0) {
        printf("Failure: Couldn't load CSV: %s\n", csv->error_message);
        return 1;
    }
    if (csv->num_records != 10) {
        printf("Failure: Didn't find 10 records\n");
        return 1;
    }

    char hello_utf8[] = {(char)0xe4, (char)0xbd, (char)0xa0, (char)0xe5, (char)0xa5, (char)0xbd, 0};
    unsigned int hello_len = ok_utf8_strlen(hello_utf8);
    uint32_t *hello = malloc(sizeof(uint32_t) * (hello_len + 1));
    ok_utf8_to_unicode(hello_utf8, hello, hello_len);
    if (!hello || hello[0] != 0x4f60 || hello[1] != 0x597d || hello[2] != 0) {
        printf("Failure: Couldn't convert UTF-8 to unicode\n");
        return 1;
    }
    free(hello);

    if (memcmp(hello_utf8, csv->fields[2][2], 7)) {
        printf("Failure: Couldn't read UTF-8 field\n");
        return 1;
    }
    if (memcmp("Smith, Fred", csv->fields[3][2], 12)) {
        printf("Failure: Couldn't read field with comma\n");
        return 1;
    }
    if (memcmp("\"The Prof\"", csv->fields[4][2], 11)) {
        printf("Failure: Couldn't read field with quotes\n");
        return 1;
    }

    if (csv->num_fields[5] != 3 || csv->fields[5][0][0] != 0 ||
        csv->fields[5][1][0] != 0 || csv->fields[5][1][0] != 0) {
        printf("Failure: Couldn't read three blank fields\n");
        return 1;
    }
    if (csv->num_fields[6] != 1 || csv->fields[6][0][0] != 0) {
        printf("Failure: Couldn't read one blank line\n");
        return 1;
    }
    if (memcmp("This is\ntwo lines", csv->fields[7][2], 18)) {
        printf("Failure: Couldn't read multiline\n");
        return 1;
    }
    if (memcmp("\nThis is also two lines", csv->fields[8][2], 24)) {
        printf("Failure: Couldn't read multiline2\n");
        return 1;
    }
    if (strlen(csv->fields[9][2]) != 4105) {
        printf("Failure: Couldn't read long line\n");
        return 1;
    }

    ok_csv_free(csv);

    printf("Success: CSV\n");
    return 0;
}

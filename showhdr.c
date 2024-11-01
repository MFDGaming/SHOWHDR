#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define GET_IMAGE_TITLE(X) ((uint8_t *)X) /* 32? */
#define BIOS_INFO_GET_LOGICAL_AREA_TYPE(X) (*((uint8_t *)X+32))
#define BIOS_INFO_GET_LOGICAL_AREA_SIZE(X) (*(uint32_t *)(((uint8_t *)X)+33))
#define BIOS_INFO_GET_LOAD_FROM_FILE(X) (*((uint8_t *)X+37))
#define BIOS_INFO_GET_REBOOT_REQUIRED(X) (*((uint8_t *)X+38))
#define BIOS_INFO_GET_UPDATE_ALL_OF_IMAGE(X) (*((uint8_t *)X+39))
#define BIOS_INFO_GET_LOGICAL_AREA_STRING(X) ((uint8_t *)X+40) /* 24? */
#define GET_TIME_STAMP(X) ((uint8_t *)X+64) /* 16? */
#define GET_THIS_FILE_START_ADDR(X) (*(uint32_t *)(((uint8_t *)X)+80))
#define GET_THIS_FILE_DATA_LENGTH(X) (*(uint32_t *)(((uint8_t *)X)+84))
#define GET_LOGICAL_AREA_TYPE(X) (*((uint8_t *)X+88))
#define GET_LAST_FILE_IN_CHAIN(X) (*((uint8_t *)X+89))
#define GET_FLASH_KEYWORD(X) ((uint8_t *)X+90) /* 6 */
#define GET_NEXT_FILE_NAME(X) ((uint8_t *)X+96) /* 16 */
#define GET_BIOS_RESERVED_STRING(X) ((uint8_t *)X+112) /* 16 */

uint8_t CalcFactoryCheckSum(uint8_t *header) {
    uint8_t chksum = 0;
    uint8_t *header_ptr;
    uint8_t savec;

    header_ptr = header;

    savec = header_ptr[79];

    header_ptr[79] = 0;

    for (int i = 0; i < 128; ++i) {
        chksum += header[0];
        ++header;
    }

    if (chksum == 0 || chksum == 1 || chksum == 255) {
        chksum = 42;
    }

    header_ptr[79] = savec;
    return chksum;
}

void print_init_help() {
    printf("%s (Show FLASH Data Image Header -- Release 1.1)\n", "SHOWHDR");
    printf("Usage: showhdr.exe <image name>\n");
}

int main(int argc, char **argv) {
    FILE *fptr;
    uint8_t *header;
    int head_mismat = 0;
    char header_string[6];

    memcpy(header_string, "FLASH", 6);

    if ((argc > 1) && (argv[1][0] == '?' || argv[1][0] == 'h' || argv[1][0] == 'H')) {
        print_init_help();
        exit(1);
    }
    if (argc < 2) {
        print_init_help();
        exit(1);
    }

    printf("%s (Show FLASH Data Image Header -- Release 1.1)\n", "SHOWHDR");

    fptr = fopen(argv[1], "rb");

    if (fptr == NULL) {
        printf("File not found\n");
        exit(1);
    }

    header = malloc(128);
    fread(header, 128, 1, fptr);

    for (int i = 0; i < 5; ++i) {
        if (GET_FLASH_KEYWORD(header)[i] != header_string[i]) {
            head_mismat = -1;
        }
    }

    if (head_mismat != 0) {
        printf("Invalid FLASH Data Image\n");
        exit(1);
    }

    printf("Image Title: %s\n", GET_IMAGE_TITLE(header));
    printf("BIOS INFO\n\tLogical Area Type: %d\n", BIOS_INFO_GET_LOGICAL_AREA_TYPE(header));
    printf("\tLogical Area Size: 0x%lx\n", BIOS_INFO_GET_LOGICAL_AREA_SIZE(header));
    if (BIOS_INFO_GET_LOAD_FROM_FILE(header) == 0) {
        printf("\tLoad From File --- : FALSE\n");
    } else {
        printf("\tLoad From File --- : TRUE\n");
    }
    if (BIOS_INFO_GET_REBOOT_REQUIRED(header) == 0) {
        printf("\tReboot Required -- : FALSE\n");
    } else {
        printf("\tReboot Required -- : TRUE\n");
    }
    if (BIOS_INFO_GET_UPDATE_ALL_OF_IMAGE(header) == '\0') {
        printf("\tUpdate All Of Image: FALSE\n");
    } else {
        printf("\tUpdate All Of Image: TRUE\n");
    }
    printf("\tLogical Area String: %s\n", BIOS_INFO_GET_LOGICAL_AREA_STRING(header));
    printf("Time Stamp --------- : %s\n", GET_TIME_STAMP(header));
    printf("This File Start Addr : 0x%lx\n", GET_THIS_FILE_START_ADDR(header));
    printf("This File Data Length: 0x%lx\n", GET_THIS_FILE_DATA_LENGTH(header));
    printf("Logical Area Type -- : %d\n", GET_LOGICAL_AREA_TYPE(header));
    if (GET_LAST_FILE_IN_CHAIN(header) == '\0') {
        printf("Last File in Chain   : FALSE\n");
    } else {
        printf("Last File in Chain   : TRUE\n");
    }
    printf("FLASH keyword ------ : %s\n", GET_FLASH_KEYWORD(header));
    printf("Next File Name ----- : %s\n", GET_NEXT_FILE_NAME(header));
    printf("BIOS Reserved String : %s\n", GET_BIOS_RESERVED_STRING(header));

    if (header[79] == CalcFactoryCheckSum(header)) {
        printf("\n\n** APPROVED FOR MANUFACTURING **\n");
    }

    return 0;
}

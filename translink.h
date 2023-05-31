#define BLANK_OFFSET 15
#define SERIAL_OFFSET 16
#define MACHINE_ID_OFFSET 16
#define EXP_DATE_OFFSET 30
#define EXP_TIME_OFFSET 44
#define ADDFARE_OFFSET 72
#define TICKET_TYPE_OFFSET 73
#define CONCESSION_OFFSET 66
#define NUM_ZONES_OFFSET 76
#define ZONE_INFO_OFFSET 78
#define ACTIVATION_DATE_OFFSET 81
#define CHECKSUM_OFFSET 108
#define DAYPASS_OFFSET 65


#define max(a,b) ((a>b)?a:b)
#define min(a,b) ((a<b)?a:b)


unsigned int readn(char *str, int n){
    unsigned int num = 0;
    int i;
    for(i=0;i<n;i++){
        num <<= 1;
        num |= str[i]-'0';
    }
    return num;
}


// FCS
static const uint16_t crc_lut[256] = {
    0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
    0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7,
    0x1081, 0x0108, 0x3393, 0x221A, 0x56A5, 0x472C, 0x75B7, 0x643E,
    0x9CC9, 0x8D40, 0xBFDB, 0xAE52, 0xDAED, 0xCB64, 0xF9FF, 0xE876,
    0x2102, 0x308B, 0x0210, 0x1399, 0x6726, 0x76AF, 0x4434, 0x55BD,
    0xAD4A, 0xBCC3, 0x8E58, 0x9FD1, 0xEB6E, 0xFAE7, 0xC87C, 0xD9F5,
    0x3183, 0x200A, 0x1291, 0x0318, 0x77A7, 0x662E, 0x54B5, 0x453C,
    0xBDCB, 0xAC42, 0x9ED9, 0x8F50, 0xFBEF, 0xEA66, 0xD8FD, 0xC974,
    0x4204, 0x538D, 0x6116, 0x709F, 0x0420, 0x15A9, 0x2732, 0x36BB,
    0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868, 0x99E1, 0xAB7A, 0xBAF3,
    0x5285, 0x430C, 0x7197, 0x601E, 0x14A1, 0x0528, 0x37B3, 0x263A,
    0xDECD, 0xCF44, 0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72,
    0x6306, 0x728F, 0x4014, 0x519D, 0x2522, 0x34AB, 0x0630, 0x17B9,
    0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5, 0xA96A, 0xB8E3, 0x8A78, 0x9BF1,
    0x7387, 0x620E, 0x5095, 0x411C, 0x35A3, 0x242A, 0x16B1, 0x0738,
    0xFFCF, 0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862, 0x9AF9, 0x8B70,
    0x8408, 0x9581, 0xA71A, 0xB693, 0xC22C, 0xD3A5, 0xE13E, 0xF0B7,
    0x0840, 0x19C9, 0x2B52, 0x3ADB, 0x4E64, 0x5FED, 0x6D76, 0x7CFF,
    0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324, 0xF1BF, 0xE036,
    0x18C1, 0x0948, 0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E,
    0xA50A, 0xB483, 0x8618, 0x9791, 0xE32E, 0xF2A7, 0xC03C, 0xD1B5,
    0x2942, 0x38CB, 0x0A50, 0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD,
    0xB58B, 0xA402, 0x9699, 0x8710, 0xF3AF, 0xE226, 0xD0BD, 0xC134,
    0x39C3, 0x284A, 0x1AD1, 0x0B58, 0x7FE7, 0x6E6E, 0x5CF5, 0x4D7C,
    0xC60C, 0xD785, 0xE51E, 0xF497, 0x8028, 0x91A1, 0xA33A, 0xB2B3,
    0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72, 0x3EFB,
    0xD68D, 0xC704, 0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232,
    0x5AC5, 0x4B4C, 0x79D7, 0x685E, 0x1CE1, 0x0D68, 0x3FF3, 0x2E7A,
    0xE70E, 0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3, 0x8238, 0x93B1,
    0x6B46, 0x7ACF, 0x4854, 0x59DD, 0x2D62, 0x3CEB, 0x0E70, 0x1FF9,
    0xF78F, 0xE606, 0xD49D, 0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330,
    0x7BC7, 0x6A4E, 0x58D5, 0x495C, 0x3DE3, 0x2C6A, 0x1EF1, 0x0F78
};

uint16_t fcs16(uint16_t crc, uint8_t *data, unsigned int len) {
    uint16_t next;
    data+=4;
    while(len--){
        next = readn((char *)data, 8);
        crc = crc_lut[(next ^ crc) & 0xff] ^ (crc >> 8);
        data += 8;
    }

    crc = crc_lut[(0 ^ crc) & 0xff] ^ (crc >> 8);
    return ~crc;
}

/*
feilds:
there are two different layouts, one for blanks and one for filled in
tickets.  the blanks have their serial number in a certain position,
but it is filled with some other number and also expiry date after
being validated.

the first known feild starts at bit position 15.

validated tickets
pos  size      usage
-=-|-=-=-=-=-|-=-=-=-=
0   (15 bits) - sentinel (000000010111001)
15  (1 bit)   - 0 if validated
16  (14 bits) - unknown (validating machine id?) - seems so.
30  (14 bits) - expiry date if validated
44  (14 bits) - expiry time if validated
58  (14 bits) - unknown (well A & A might know) Possibly payment method.
72  (1 bit)   - 1 if addfare
73  (3 bits)  - ticket type (consession or adult)
76  (2 bits)  - number of zones that this ticket is valid for.
78  (3 bits)  - zone the ticket was issued in.
81  (14 bits) - unknown (date?)
95  (13 bits) - zeros
108 (16 bits) - checksum
126 (4 bits)  - zeros

blank ticket
pos  size      usage
-=-|-=-=-=-=-|-=-=-=-=
0   (15 bits) - sentinel (000000010111001)
15  (1 bit)   - 1 if blank
16  (32 bits) - serial if blank
48  (28 bits) - zeros  (well, maybe the ticket type might be at 73,
but we don't have any concession blanks.)
76  (2 bits)  - number of zones that this ticket is valid for.
78  (15 bits) - All ones.
108 (16 bits) - checksum
126 (3 bits)  - zeros
 */

void superimpose(char *bits, unsigned int val, int offset, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        bits[offset + i] = !!(val & (1 << (len - i - 1))) + '0';
    }
}

char *encode(struct tm tm, int z_issued, int n_zones){
    //static char bits[] = "00000001011100100001010110110101000100010001010111110000000000000000000000010110001000100010011000000000000000110111100110000000";
    static char bits[] = "00000001011100100001000001011101000110010100000110000000000100000000000000111100101000110010110000000000000011001011000100010000";
    struct tm epoch = {0};

    tm.tm_isdst = -1;

    epoch.tm_mday = 31;
    epoch.tm_mon = 12 - 1; /* 0 - 11 */
    epoch.tm_year = 1999 - 1900;
    epoch.tm_hour = 17;
    epoch.tm_min = 36;

    time_t now_time   = mktime(&tm);
    time_t epoch_time = mktime(&epoch);

    int secs = difftime(now_time, epoch_time);
    int mins = secs / 60;
    int expdate = mins / (60 * 24);
    int exptime = mins % (60 * 24) / 6;

    z_issued = 1 << (z_issued - 1);

    superimpose(bits, expdate, EXP_DATE_OFFSET, 14);
    superimpose(bits, exptime, EXP_TIME_OFFSET, 8);
    superimpose(bits, z_issued, ZONE_INFO_OFFSET, 3);
    superimpose(bits, n_zones, NUM_ZONES_OFFSET, 2);
    superimpose(bits, n_zones, TICKET_TYPE_OFFSET, 3);
    superimpose(bits, expdate + 2, ACTIVATION_DATE_OFFSET, 14);

    uint16_t calculated = fcs16(0xffff, (uint8_t *)bits, 12);

    superimpose(bits, calculated, CHECKSUM_OFFSET, 16);

    return bits;
}

int reformat(char *bits, uint8_t *bytes){
    int count = 0;
    bits += strspn(bits, "0");
    int header_len = 7;


    while (*bits) {
        if (strchr(bits, '1') == NULL) break;
        int len = min(8, strlen(bits));
        uint8_t byte = readn(bits, len);
        byte = (byte * 0x0202020202ULL & 0x010884422010ULL) % 1023;
        bytes[header_len + count++] = byte;
        bits += len;
    }

    bytes[0] = 0x1b;
    bytes[1] = 0x6e;
    bytes[2] = 0x1b;
    bytes[3] = 0x73;
    bytes[4] = 0x1b;
    bytes[5] = 0x02;
    bytes[6] = count;
    bytes[header_len + count + 0] = 0x3f;
    bytes[header_len + count + 1] = 0x1c;

    return header_len + count + 2;
}

int unformat(uint8_t *bytes, int len, char *bits){
    int i = 0;
    int j;
    if (bytes[0] == 0x1b){
        i = 7;
        len -= 2;
    }
    strcpy(bits, "0000000");
    int idx = 7;
    for(;i<len;i++){
        for(j=0;j<8;j++){
            bits[idx++] = !!(bytes[i] & (1 << j)) + '0';
        }
    }
    bits[idx] = '\0';
    return idx;
}

const char *machinename(int machine_id){
    const char *expo[] = {"Waterfront", "Burrard", "Granville", "Stadium", "Main St", "Broadway", "Nanaimo", "29th Ave", "Joyce", "Patterson", "Metrotown", "Royal Oak", "Edmonds", "22nd St", "New West", "Columbia", "Scott Road", "Gateway", "Surrey Central", "King George"};
    const char *mill[] = {"Commercial", "Renfrew", "Rupert", "Gilmore", "Brentwood", "Holdom", "Sperling", "Lake City Way", "Production Way", "Lougheed", "Braid", "Sapperton"};
    const char *canada[] = {"Waterfront", "Vancouver City Centre", "Yaletown", "Olympic Village", "Broadway", "King Edward", "Oakridge", "Langara", "Marine Drive", "Bridgeport", "Templeton", "Sea Island Centre", "YVR-Airport", "Aberdeen", "Landsdowne", "Richmond-Brighouse"};
    struct {
        int id;
        const char *name;
    } routes[] = {
        {8,    "041 JOYCE STN/CROWN/UBC?"},
        {335,  "also 135"},
        {1046, "016 29TH AVENUE STN/ARBUTUS"},
        {1047, "014 HASTINGS/UBC?"},
        {1274, "135 SFU/BURRARD STN"},
        {1262, "002 MACDONALD-16TH AVE/BURRARD STN"},
        {10031,"Joyce Skytrain Stn?"},
    };

    if ((10001 <= machine_id) && (machine_id <= 10020)){
        return expo[machine_id - 10001];
    } else if ((10032 <= machine_id) && (machine_id <= 10043)){
        return mill[machine_id - 10032];
    } else if ((50 <= machine_id) && (machine_id <= 65)){
        return canada[machine_id - 50];
    } else {
        int i;
        for(i=0;i<sizeof(routes)/sizeof(routes[0]);i++){
            if (machine_id == routes[i].id){
                return routes[i].name;
            }
        }
        return "Unknown bus route?";
    }
}
int decode(char *str){
    struct tm time;
    memset(&time, '\0', sizeof(time));
    char buf[64];
    int extrayears = 0;

    int blank = readn(str + BLANK_OFFSET, 1);
    int serial = readn(str + SERIAL_OFFSET, 32);
    int machine_id = readn(str + MACHINE_ID_OFFSET, 14);
    int expdate = readn(str + EXP_DATE_OFFSET, 14);
    int exptime = readn(str + EXP_TIME_OFFSET, 8);
    int unknown_time = readn(str + EXP_TIME_OFFSET + 8, 8);
    int addfare = readn(str + ADDFARE_OFFSET, 1);
    int ticket_type = readn(str + TICKET_TYPE_OFFSET, 3);
    int num_zones = readn(str + NUM_ZONES_OFFSET, 2);
    int zone_info = readn(str + ZONE_INFO_OFFSET, 3);
    int activation_date = readn(str + ACTIVATION_DATE_OFFSET, 14);
    int daypass = readn(str + DAYPASS_OFFSET, 1);
    int concession = readn(str + CONCESSION_OFFSET, 1);
    uint16_t checksum = readn(str + CHECKSUM_OFFSET, 16);

#if 0
    {
        printf("%d %s\n", machine_id, machinename(machine_id));
        uint16_t calculated2 = fcs16(0xffff, (uint8_t *)str, 12);
        if (calculated2 != checksum){
            return 1;
        } else {
            return 0;
        }
    }
#endif

    time.tm_isdst = -1;
    time.tm_mday = 31;
    time.tm_mon = 12 - 1; /* 0 - 11 */
    time.tm_year = 1999-1900;
    time.tm_mday += expdate;

    time.tm_hour = 17;
    time.tm_min = 36;

    if (time.tm_isdst){
        time.tm_hour = 18;
    }

    time.tm_hour += exptime / 10;
    time.tm_min += (exptime % 10) * 6;
    if (time.tm_mday > 14000){ /* days between 1999 and 2038 */
        extrayears = time.tm_mday / 1461;
        extrayears *= 4;
        time.tm_mday %= 1461;
    } 

    mktime(&time);
    time.tm_year += extrayears;

    strftime(buf, sizeof(buf), "%a, %b %d %Y %I:%M %p", &time);

    {
        time.tm_hour = 17;
        time.tm_min = 36;
        if (time.tm_isdst){
            time.tm_hour = 18;
        }
        time.tm_hour += unknown_time / 10;
        time.tm_min += (unknown_time % 10) * 6;
        char buf2[64];
        mktime(&time);
        strftime(buf2, sizeof(buf2), "%I:%M %p", &time);
        printf("unknown time: %s\n", buf2);
    }


    const char *zone_issued = "[ XXX ]";
    switch (zone_info){
    case 0b001:
        zone_issued = "[ -1- ]";
        break;
    case 0b010:
        zone_issued = "[ -2- ]";
        break;
    case 0b100:
    case 0b111:
        zone_issued = "[ -3- ]";
        break;
    case 0b011:
        zone_issued = "[-2-A?]";
        break;
    default:
        printf("unknown zone: %#x\n", zone_info);
        break;
    }

    if (concession){
        printf("Concession ");
    } else {
        printf("Adult ");
    }

    if (daypass){
        printf("DAYPASS ");
    }

    if (blank){
        printf("Blank %d zone faresaver, serial number %d-%d\n", num_zones, serial / 10, serial % 10);
    } else {
        printf("%s, %d zone(s) type: %d issued zone: %s addfare: %s machine id: %d (%s) ", buf, num_zones, ticket_type, zone_issued, (addfare)?"[true ]":"[false]", machine_id, machinename(machine_id));
        if (activation_date == (1<<14)-1){
            printf("ticket type: faresaver\n");
        } else {
            memset(&time, 0, sizeof(time));
            time.tm_mday = 31;
            time.tm_mon = 11;
            time.tm_year = 1999-1900;
            time.tm_mday += activation_date;
            mktime(&time);
            strftime(buf, sizeof(buf), "%a, %b %d %Y", &time);
            printf("ticket type: transfer, activated %s\n", buf);
        }
    }

    uint16_t calculated = fcs16(0xffff, (uint8_t *)str, 12);
    if (calculated != checksum){
        printf("checksum error: ticket says %04hx, calculated %04hx\n", checksum, calculated);
        return 1;
    } else {
        printf("checksum OK: %04hx\n", calculated);
        return 0;
    }
}

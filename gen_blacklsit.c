#include <stdio.h>

typedef struct {
    char version[11];
    unsigned short total_record;
    unsigned char nuber_bytes;
    unsigned char report_cnt_bytes;
    unsigned char tag_bytes;
}BLACKLIST_HEAD;

typedef struct {
    unsigned char tel[14];
    unsigned short report;
    unsigned short tag;
}RECORD_S;

typedef unsigned short kal_uint16 ;
typedef unsigned char kal_uint8  ;
typedef enum 
{
    /* FALSE value */
    KAL_FALSE,
    /* TRUE value */
    KAL_TRUE
} kal_bool;

RECORD_S records[] = 
{
    {"13725543246", 3, 0x1},
    {"13725543247", 343, 0x2},
    {"13735543246", 56, 0x3},
};

kal_uint8 applib_addr_bcd_char_2_digit(kal_uint8 ch)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* input digit is a phone number digit */
    if (ch == '*')
    {
        return 0xa;
    }
    else if (ch == '#')
    {
        return 0xb;
    }
    else if (ch == 'p')
    {
        return 0xc;
    }
    else if (ch == 'w')
    {
        return 0xd;
    }
    else if (ch >= '0' && ch <= '9')
    {
        return (ch - '0');
    }

    return (char)0xFF;
}

void applib_addr_string_2_bcd(char *src, kal_uint16 src_size, kal_uint8 *dst, kal_uint16 dst_size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i, j = 0;
    kal_uint8 byte = 0;
    kal_bool is_lhbyte = KAL_FALSE;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (strlen(src) < src_size || dst == NULL || dst_size == 0)
    {
        return;
    }
    
    for (i = 0; i < src_size && j < dst_size; i++)
    {
        if ((byte = applib_addr_bcd_char_2_digit(src[i])) == 0xFF)
        {
            continue;
        }

        if (!is_lhbyte)
        {
            dst[j] = byte;
            is_lhbyte = KAL_TRUE;
        }
        else
        {
            dst[j++] |= (char)(byte << 4);
            is_lhbyte = KAL_FALSE;
        }
    }

    if (is_lhbyte)
    {
        dst[j] |= 0xF0;
    }
}

void main(void)
{
    BLACKLIST_HEAD head1 = {0};
    RECORD_S  record;
    int i = 0;
    FILE *fp;


    fp = fopen("blacklist.bin", "wb");

    head1.total_record = sizeof(records)/sizeof(records[0]);
    head1.nuber_bytes = 14;
    head1.report_cnt_bytes = 2;
    head1.tag_bytes = 2;
    strcpy(head1.version, "46016110502");

//字节对齐原因，分次写
    fwrite(&head1.version, sizeof(head1.version), 1, fp);
    fwrite(&head1.total_record, sizeof(head1.total_record), 1, fp);
    fwrite(&head1.nuber_bytes, sizeof(head1.nuber_bytes), 1, fp);
    fwrite(&head1.report_cnt_bytes, sizeof(head1.report_cnt_bytes), 1, fp);
    fwrite(&head1.tag_bytes, sizeof(head1.tag_bytes), 1, fp);
    
    for (i=0;i<head1.total_record;i++)
    {
        record= records[i];
        memset(record.tel, 0 ,14);
        applib_addr_string_2_bcd(records[i].tel, strlen(records[i].tel), record.tel, 14);
        fwrite(&record.tel, sizeof(record.tel), 1, fp);
        fwrite(&record.report, sizeof(record.report), 1, fp);
        fwrite(&record.tag, sizeof(record.tag), 1, fp);
    }

	
    fclose(fp);
}

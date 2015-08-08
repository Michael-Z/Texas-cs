#ifdef __cplusplus
extern "C"
{
#endif

    char* Itoa( int value, char* result, int base )
    {
        if (base < 2 || base > 16)
        {
            *result = 0;
            return result;
        }
        char* out = result;
        int quotient = value;

        int absQModB;

        do
        {
            absQModB=quotient % base;
            if (absQModB < 0)
                absQModB=-absQModB;
            *out = "0123456789abcdef"[ absQModB ];
            ++out;
            quotient /= base;
        }
        while ( quotient );

        if ( value < 0 && base == 10) *out++ = '-';

        *out = 0;

        char *start = result;
        char temp;
        out--;
        while (start < out)
        {
            temp=*start;
            *start=*out;
            *out=temp;
            start++;
            out--;
        }

        return result;
    }

#ifdef __cplusplus
}
#endif

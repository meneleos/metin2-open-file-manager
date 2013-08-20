#include "LZO.h"
#include "Standards.h"
#include <iostream>

int LZO::Decompress(const unsigned char* In , unsigned long InSize, unsigned char* Out, unsigned long* OutSize)
{
	register unsigned char* op;
    register const unsigned char* ip;
    register unsigned long t;
    register const unsigned char* m_pos;

    const unsigned char* const ip_end = In + InSize;
    unsigned char* const op_end = Out + *OutSize;

    *OutSize = 0;

    op = Out;
    ip = In;

    if (*ip > 17)
    {
        t = *ip++ - 17;
        if (t < 4) goto match_next;
        do *op++ = *ip++; while (--t > 0);
        goto first_literal_run;
    }

    while (ip < ip_end)
    {
        t = *ip++;
        if (t >= 16)
            goto match;
        if (t == 0)
        {
            while (*ip == 0)
            {
                t += 255;
                ip++;
            }
            t += 15 + *ip++;
        }
		((* (volatile unsigned int*) (volatile void*) (op)) = (unsigned long) (* (volatile const unsigned int*) (volatile const void*) (ip)));
        op += 4; ip += 4;
        if (--t > 0)
        {
            if (t >= 4)
            {
                do {
                    ((* (volatile unsigned int*) (volatile void*) (op)) = (unsigned long) (* (volatile const unsigned int*) (volatile const void*) (ip)));
                    op += 4; ip += 4; t -= 4;
                } while (t >= 4);
                if (t > 0) do *op++ = *ip++; while (--t > 0);
            }
            else
                do *op++ = *ip++; while (--t > 0);
        }

first_literal_run:

        t = *ip++;
        if (t >= 16) goto match;
        m_pos = op - (1 + 0x0800);
        m_pos -= t >> 2;
        m_pos -= *ip++ << 2;
        *op++ = *m_pos++; *op++ = *m_pos++; *op++ = *m_pos;
        goto match_done;

        do {
match:
            if (t >= 64)
            {
                m_pos = op - 1;
                m_pos -= (t >> 2) & 7;
                m_pos -= *ip++ << 3;
                t = (t >> 5) - 1;
                goto copy_match;
            }
            else if (t >= 32)
            {
                t &= 31;
                if (t == 0)
                {
                    while (*ip == 0)
                    {
                        t += 255;
                        ip++;
                    }
                    t += 31 + *ip++;
                }
                m_pos = op - 1;
                m_pos -= (* (volatile const unsigned short*) (volatile const void*) (ip)) >> 2;
                ip += 2;
            }
            else if (t >= 16)
            {
                m_pos = op;
                m_pos -= (t & 8) << 11;
                t &= 7;
                if (t == 0)
                {
                    while (*ip == 0)
                    {
                        t += 255;
                        ip++;
                    }
                    t += 7 + *ip++;
                }
                m_pos -= (* (volatile const unsigned short*) (volatile const void*) (ip)) >> 2;
                ip += 2;
                if (m_pos == op)
                    goto eof_found;
                m_pos -= 0x4000;
            }
            else
            {
                m_pos = op - 1;
                m_pos -= t >> 2;
                m_pos -= *ip++ << 2;
                *op++ = *m_pos++; *op++ = *m_pos;
                goto match_done;
            }

            if (t >= 2 * 4 - (3 - 1) && (op - m_pos) >= 4)
            {
				((* (volatile unsigned int*) (volatile void*) (op)) = (unsigned long) (* (volatile const unsigned int*) (volatile const void*) (m_pos)));
                op += 4; m_pos += 4; t -= 4 - (3 - 1);
                do {
                    ((* (volatile unsigned int*) (volatile void*) (op)) = (unsigned long) (* (volatile const unsigned int*) (volatile const void*) (m_pos)));
                    op += 4; m_pos += 4; t -= 4;
                } while (t >= 4);
                if (t > 0) do *op++ = *m_pos++; while (--t > 0);
            }
            else
            {
copy_match:
                *op++ = *m_pos++; *op++ = *m_pos++;
                do *op++ = *m_pos++; while (--t > 0);
            }


match_done:
            t = ip[-2] & 3;
            if (t == 0)
                break;

match_next:
            *op++ = *ip++;
            if (t > 1) {
				*op++ = *ip++;
				if (t > 2) 
				{
					*op++ = *ip++;
				}
			}
            t = *ip++;
        } while (ip < ip_end);
    }
    *OutSize = ((unsigned long) ((op)-(Out)));
    return LZO_E_EOF_NOT_FOUND;

eof_found:
    *OutSize = ((unsigned long) ((op)-(Out)));
    return (ip == ip_end ? LZO_E_OK :
           (ip < ip_end  ? LZO_E_INPUT_NOT_CONSUMED : LZO_E_INPUT_OVERRUN));
}

unsigned long LZO::CompWrap(const unsigned char* in ,  unsigned long  in_len, unsigned char* out,  unsigned long* out_len,  unsigned long  ti,  void* wrkmem)
{
	register const unsigned char* ip;
    unsigned char* op;
    const unsigned char* const in_end = in + in_len;
    const unsigned char* const ip_end = in + in_len - 20;
    const unsigned char* ii;
    unsigned short* const dict = (unsigned short*)wrkmem;

    op = out;
    ip = in;
    ii = ip;

    ip += ti < 4 ? 4 - ti : 0;
    for (;;)
    {
        const unsigned char* m_pos;
        unsigned long m_off;
        unsigned long m_len;
        {
        int dv;
        unsigned long dindex;
literal:
        ip += 1 + ((ip - ii) >> 5);
next:
        if (ip >= ip_end)
            break;
        dv = (* (volatile const int*) (volatile const void*) (ip));
        dindex = ((unsigned long) (((((((unsigned long) ((0x1824429d) * (dv)))) >> (32-14))) & ( ((1u << 14) - 1) >> (0))) << (0)));
        m_pos = in+dict[dindex];
        dict[dindex] = ((unsigned short)  ((unsigned long) ((ip)-(in))));
        if (dv != (* (volatile const int*) (volatile const void*) (m_pos)))
            goto literal;
        }

        ii -= ti; ti = 0;
        {
        register unsigned long t = (unsigned long) ((ip)-(ii));
        if (t != 0)
        {
            if (t <= 3)
            {
                op[-2] |= (unsigned char)(t);
                ((* (volatile int*) (volatile void*) (op)) = (int) ((* (volatile const int*) (volatile const void*) (ii))));
                op += t;
            }
            else if (t <= 16)
            {
                *op++ = (unsigned char)(t - 3);
                ((* (volatile int*) (volatile void*) (op)) = (int) ((* (volatile const int*) (volatile const void*) (ii))));
                ((* (volatile int*) (volatile void*) (op+4)) = (int) ((* (volatile const int*) (volatile const void*) (ii+4))));
                ((* (volatile int*) (volatile void*) (op+8)) = (int) ((* (volatile const int*) (volatile const void*) (ii+8))));
                ((* (volatile int*) (volatile void*) (op+12)) = (int) ((* (volatile const int*) (volatile const void*) (ii+12))));
                op += t;
            }
            else
            {
                if (t <= 18)
                    *op++ = (unsigned char)(t - 3);
                else
                {
                    register unsigned long tt = t - 18;
                    *op++ = 0;
                    while (tt > 255)
                    {
                        tt -= 255;
                        * (volatile unsigned char *) op++ = 0;
                    }
                    *op++ = (unsigned char)(tt);
                }
                do {
                    ((* (volatile int*) (volatile void*) (op)) = (int) ((* (volatile const int*) (volatile const void*) (ii))));
                    ((* (volatile int*) (volatile void*) (op+4)) = (int) ((* (volatile const int*) (volatile const void*) (ii+4))));
                    ((* (volatile int*) (volatile void*) (op+8)) = (int) ((* (volatile const int*) (volatile const void*) (ii+8))));
                    ((* (volatile int*) (volatile void*) (op+12)) = (int) ((* (volatile const int*) (volatile const void*) (ii+12))));
                    op += 16; ii += 16; t -= 16;
                } while (t >= 16); if (t > 0)
                { do *op++ = *ii++; while (--t > 0); }
            }
        }
        }
        m_len = 4;
        {
        int v;
        v = (* (volatile const int*) (volatile const void*) (ip + m_len)) ^ (* (volatile const int*) (volatile const void*) (m_pos + m_len));
        if (v == 0) {
            do {
                m_len += 4;
                v = (* (volatile const int*) (volatile const void*) (ip + m_len)) ^ (* (volatile const int*) (volatile const void*) (m_pos + m_len));
                if (ip + m_len >= ip_end)
                    goto m_len_done;
            } while (v == 0);
        }
		unsigned long res;
        m_len += res / 8;
        }
m_len_done:
        m_off = (unsigned long) ((ip)-(m_pos));
        ip += m_len;
        ii = ip;
        if (m_len <= 8 && m_off <= 0x0800)
        {
            m_off -= 1;
            *op++ = (unsigned char)(((m_len - 1) << 5) | ((m_off & 7) << 2));
            *op++ = (unsigned char)(m_off >> 3);
        }
        else if (m_off <= 0x4000)
        {
            m_off -= 1;
            if (m_len <= 33)
                *op++ = (unsigned char)(32 | (m_len - 2));
            else
            {
                m_len -= 33;
                *op++ = 32 | 0;
                while (m_len > 255)
                {
                    m_len -= 255;
                    * (volatile unsigned char *) op++ = 0;
                }
                *op++ = (unsigned char)(m_len);
            }
            *op++ = (unsigned char)(m_off << 2);
            *op++ = (unsigned char)(m_off >> 6);
        }
        else
        {
            m_off -= 0x4000;
            if (m_len <= 9)
                *op++ = (unsigned char)(16 | ((m_off >> 11) & 8) | (m_len - 2));
            else
            {
                m_len -= 9;
                *op++ = (unsigned char)(16 | ((m_off >> 11) & 8));
                while (m_len > 255)
                {
                    m_len -= 255;
                    * (volatile unsigned char *) op++ = 0;
                }
                *op++ = (unsigned char)(m_len);
            }
            *op++ = (unsigned char)(m_off << 2);
            *op++ = (unsigned char)(m_off >> 6);
        }
        goto next;
    }

    *out_len = (unsigned long) ((op)-(out));
    return (unsigned long) ((in_end)-(ii-ti));
}

int LZO::Compress(const unsigned char* In , unsigned long InSize, unsigned char* Out, unsigned long* OutSize)
{
	const unsigned char* ip = In;
    unsigned char* op = Out;
    unsigned long l = InSize;
    unsigned long t = 0;
    void* wrkmem = new void*[16384];

    while (l > 20)
    {
        unsigned long ll = l;
        size_t ll_end;
        ll = ((ll) <= (49152) ? (ll) : (49152));
        ll_end = (size_t)ip + ll;
        if ((ll_end + ((t + ll) >> 5)) <= ll_end || (const unsigned char*)(ll_end + ((t + ll) >> 5)) <= ip + ll) break;
        memset(wrkmem, 0, ((unsigned long)1 << 14) * sizeof(unsigned short));
        t = this->CompWrap(ip,ll,op,OutSize,t,wrkmem);
        ip += ll;
        op += *OutSize;
        l  -= ll;
    }
    t += l;

    if (t > 0)
    {
        const unsigned char* ii = In + InSize - t;

        if (op == Out && t <= 238)
            *op++ = (unsigned char)(17 + t);
        else if (t <= 3)
            op[-2] |= (unsigned char)(t);
        else if (t <= 18)
            *op++ = (unsigned char)(t - 3);
        else
        {
            unsigned long tt = t - 18;

            *op++ = 0;
            while (tt > 255)
            {
                tt -= 255;

                * (volatile unsigned char *) op++ = 0;
            }
            *op++ = (unsigned char)(tt);
        }
        do *op++ = *ii++; while (--t > 0);
    }

    *op++ = 16 | 1;
    *op++ = 0;
    *op++ = 0;

    *OutSize = (unsigned long) ((op)-(Out));
    return LZO_E_OK;
}
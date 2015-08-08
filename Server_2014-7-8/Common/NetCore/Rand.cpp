#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Rand.h"

#define N        (624)
#define M        (397)
#define K        (0x9908B0DFU)
#define hiBit(u)       ((u) & 0x80000000U)
#define loBit(u)       ((u) & 0x00000001U)
#define loBits(u)      ((u) & 0x7FFFFFFFU)
#define mixBits(u, v)  (hiBit(u)|loBits(v))

static unsigned int _state[ N + 1 ];
static unsigned int *_next;
static int _left = -1;

using namespace VENet;

void seedMT( unsigned int seed, unsigned int *state, unsigned int *&next, int &left );
unsigned int reloadMT( unsigned int *state, unsigned int *&next, int &left );
unsigned int randomMT( unsigned int *state, unsigned int *&next, int &left );
void fillBufferMT( void *buffer, unsigned int bytes, unsigned int *state, unsigned int *&next, int &left );
float frandomMT( unsigned int *state, unsigned int *&next, int &left );

void seedMT( unsigned int seed )
{
    seedMT(seed, _state, _next, _left);
}
unsigned int reloadMT( void )
{
    return reloadMT(_state, _next, _left);
}
unsigned int randomMT( void )
{
    return randomMT(_state, _next, _left);
}
float frandomMT( void )
{
    return frandomMT(_state, _next, _left);
}
void fillBufferMT( void *buffer, unsigned int bytes )
{
    fillBufferMT(buffer, bytes, _state, _next, _left);
}

void seedMT( unsigned int seed, unsigned int *state, unsigned int *&next, int &left )   // Defined in cokus_c.c
{
    (void) next;

    register unsigned int x = ( seed | 1U ) & 0xFFFFFFFFU, *s = state;
    register int j;

    for ( left = 0, *s++ = x, j = N; --j;
            *s++ = ( x *= 69069U ) & 0xFFFFFFFFU )

        ;
}


unsigned int reloadMT( unsigned int *state, unsigned int *&next, int &left )
{
    register unsigned int * p0 = state, *p2 = state + 2, *pM = state + M, s0, s1;
    register int j;

    if ( left < -1 )
        seedMT( 4357U );

    left = N - 1, next = state + 1;

    for ( s0 = state[ 0 ], s1 = state[ 1 ], j = N - M + 1; --j; s0 = s1, s1 = *p2++ )
        * p0++ = *pM++ ^ ( mixBits( s0, s1 ) >> 1 ) ^ ( loBit( s1 ) ? K : 0U );

    for ( pM = state, j = M; --j; s0 = s1, s1 = *p2++ )
        * p0++ = *pM++ ^ ( mixBits( s0, s1 ) >> 1 ) ^ ( loBit( s1 ) ? K : 0U );

    s1 = state[ 0 ], *p0 = *pM ^ ( mixBits( s0, s1 ) >> 1 ) ^ ( loBit( s1 ) ? K : 0U );

    s1 ^= ( s1 >> 11 );

    s1 ^= ( s1 << 7 ) & 0x9D2C5680U;

    s1 ^= ( s1 << 15 ) & 0xEFC60000U;

    return ( s1 ^ ( s1 >> 18 ) );
}


unsigned int randomMT( unsigned int *state, unsigned int *&next, int &left )
{
    unsigned int y;

    if ( --left < 0 )
        return ( reloadMT(state, next, left) );

    y = *next++;

    y ^= ( y >> 11 );

    y ^= ( y << 7 ) & 0x9D2C5680U;

    y ^= ( y << 15 ) & 0xEFC60000U;

    return ( y ^ ( y >> 18 ) );

}

void fillBufferMT( void *buffer, unsigned int bytes, unsigned int *state, unsigned int *&next, int &left )
{
    unsigned int offset=0;
    unsigned int r;
    while (bytes-offset>=sizeof(r))
    {
        r = randomMT(state, next, left);
        memcpy((char*)buffer+offset, &r, sizeof(r));
        offset+=sizeof(r);
    }

    r = randomMT(state, next, left);
    memcpy((char*)buffer+offset, &r, bytes-offset);
}

float frandomMT( unsigned int *state, unsigned int *&next, int &left )
{
    return ( float ) ( ( double ) randomMT(state, next, left) / 4294967296.0 );
}
VENetRandom::VENetRandom()
{
    left=-1;
}
VENetRandom::~VENetRandom()
{
}
void VENetRandom::SeedMT( unsigned int seed )
{
    printf("%i\n",seed);
    seedMT(seed, state, next, left);
}

unsigned int VENetRandom::ReloadMT( void )
{
    return reloadMT(state, next, left);
}

unsigned int VENetRandom::RandomMT( void )
{
    return randomMT(state, next, left);
}

float VENetRandom::FrandomMT( void )
{
    return frandomMT(state, next, left);
}

void VENetRandom::FillBufferMT( void *buffer, unsigned int bytes )
{
    fillBufferMT(buffer, bytes, state, next, left);
}
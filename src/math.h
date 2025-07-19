// simple math equations

#define NULL 0

int abs(int x)
{
    return x < 0 ? x * -1 : x;
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int min(int a, int b)
{
    return (a < b) ? a : b;
}

float fabs (float x)
{
    return x < 0.0 ? -1 * x : x;
}
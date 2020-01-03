#include "io.h"
#include <pthread.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

enum Mod
{
    m_decrease,
    m_increase,
};
typedef struct { float *mIn, *mOut; int x, y, w, h, a, b; enum Mod mod; } ThreadData;

float getM(float *m, int w, int x, int y) { return m[x + y * w]; }
void setM(float *m, int w, int x, int y, float val)
{
    m[x + y * w] = val;
}

void* filter(void *arg)
{
    ThreadData td = *((ThreadData*)arg);
    float val = getM(td.mIn, td.w, td.x, td.y), t;

    for (int i = td.x - td.a; i <= td.x + td.a; i++)
        for (int j = td.y - td.b; j <= td.y + td.b; j++)
        {
            if (i < 0 || j < 0 || i >= td.w || j >= td.h) continue;
            t = getM(td.mIn, td.w, i, j);

            if (td.mod == m_decrease)
            {
                if (t < val) val = t;
            }
            else
            {
                if (t > val) val = t;
            }
        }

    setM(td.mOut, td.w, td.x, td.y, val);
    //writeString("Thread: "); writeInt(td.x + td.y * td.w); writeChar('\n');
}

void calculate(float *mD1, float *mI1, float *mD2, float *mI2, int w, int h, int a, int b, int k, int mt)
{
    int wh = w * h, sz = 2 * wh;
    if (mt <= 0 || mt > sz) mt = sz;
    int mtStart = mt;

    ThreadData td, tdd[sz];
    pthread_t threads[sz];

    td.a = a;
    td.b = b;
    td.w = w;
    td.h = h;

    for (int kk = 0; kk < k; kk++)
    {
        //init struct
        td.mIn = mD1;
        td.mOut = mD2;
        td.mod = m_decrease;

        mt = mtStart;

        //start threading
        int iter = 0;
        while (iter < sz)
        {
            mt = min(mt, sz - iter);
            for (int m = 0; m < mt; m++)
            {
                if (iter == wh)
                {
                    td.mIn = mI1;
                    td.mOut = mI2;
                    td.mod = m_increase;
                }

                td.x = iter % wh % w; td.y = iter % wh / w;
                iter++;

                tdd[m] = td;
                if (pthread_create(threads + m, NULL, filter, (void*)(tdd + m)))
                {
                    free(mD1); free(mD2); free(mI1); free(mI2);
                    error("cannot create thread!", -10);
                }
            }
            //join
            for (int m = 0; m < mt; m++) pthread_join(threads[m], NULL);
            //writeInt(mt); writeString("!\n");
        }
        //copy result
        for (int i = 0; i < wh; i++)
        {
            mD1[i] = mD2[i];
            mI1[i] = mI2[i];
        }
    }
}

int main(int argc, char **argv)
{
    int maxThread = 0, afterPoint = 3;
    if (argc > 1) 
    {
        afterPoint = atoi(argv[1]);
        if (argc > 2) maxThread = atoi(argv[2]);
    }

    //read matrix
    writeString("Write size of matrix (width and height): ");

    int w = readInt(), h = readInt(), wh = w * h; //size of matrix
    if (w <= 0) error("width must be positive!", -1);
    if (h <= 0) error("height must be positive!", -2);

    writeString("Width: "); writeInt(w); writeChar('\n');
    writeString("Height: "); writeInt(h); writeChar('\n');

    float *mDecrease, *mIncrease, *mDecrease2, *mIncrease2;
    mDecrease = malloc(wh * sizeof(float)); //create erosion matrix
    if (!mDecrease) error("cannot allocate memory for matrix!", -3);
    mIncrease = malloc(wh * sizeof(float)); //create increasing matrix
    if (!mIncrease) { free(mDecrease); error("cannot allocate memory for matrix!", -4); }
    mDecrease2 = malloc(wh * sizeof(float)); //create erosion matrix copy
    if (!mDecrease2) { free(mDecrease); free(mIncrease); error("cannot allocate memory for matrix!", -5); }
    mIncrease2 = malloc(wh * sizeof(float)); //create increasing matrix copy
    if (!mIncrease2) { free(mDecrease); free(mIncrease); free(mDecrease2); error("cannot allocate memory for matrix!", -6); }

    writeString("Write elements of matrix:\n"); 
    for (int i = 0; i < wh; i++) mDecrease[i] = mIncrease[i] = readFloat(); //read matrix

    //show matrix
    writeString("Inputed matrix:\n"); 
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++) { writeFloat(getM(mDecrease, w, i, j), afterPoint); writeChar(' '); }
        writeChar('\n');
    }
    //filter setup
    writeString("Input half-width and half-height of filter matrix: ");
    int a = readInt(), b = readInt();
    if (a < 0) { free(mDecrease); free(mIncrease); free(mDecrease2); free(mIncrease2); error("width must be non-negative!", -7); }
    if (b < 0) { free(mDecrease); free(mIncrease); free(mDecrease2); free(mIncrease2); error("height must be non-negative!", -8); }

    writeString("K = ");
    int k = readInt();
    if (k < 0) { free(mDecrease); free(mIncrease); free(mDecrease2); free(mIncrease2); error("K must be non-negative!", -9); }

    //calculate matrixes
    calculate(mDecrease, mIncrease, mDecrease2, mIncrease2,w, h, a, b, k, maxThread);

    //show matrixes
    writeString("Erosion matrix:\n"); 
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++) { writeFloat(getM(mDecrease, w, i, j), afterPoint); writeChar(' '); }
        writeChar('\n');
    }
    writeString("Increasing matrix:\n"); 
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++) { writeFloat(getM(mIncrease, w, i, j), afterPoint); writeChar(' '); }
        writeChar('\n');
    }

    free(mDecrease); //free memory
    free(mIncrease); //free memory
    free(mDecrease2); //free memory
    free(mIncrease2); //free memory
    return 0;
}
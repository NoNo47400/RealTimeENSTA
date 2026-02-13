int main() {
    volatile int* pFifo = (int*) 0xFC000000; // We say to the compiler that the memory can change outside the program (this allows rewriting to the same address several times without the compiler simplifying to a single write (the last one))

    for (int i=0; i<1000; i++) {
        pFifo[0]= 2*i+1;
    }

    int size =pFifo[2];
    int* tab = new int[size];
    
    for (int i=0; i<size; i++) {
        tab[i]= pFifo[1];
    }

    delete[] tab;
    
    return 0;
}
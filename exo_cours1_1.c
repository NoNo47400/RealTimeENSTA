unsigned* READ_ADDR = (unsigned*)0xFC000000;
unsigned* WRITE_ADDR = (unsigned*)0xFC000004;
unsigned* PORT_COUNT_ADDR = (unsigned*)0xFC000008;

int main() {
    // On écrit que des chiffres impairs dans la mémoire
    for (int i = 0; i < 2000; i++) {
        if (i % 2 != 0) {
            *WRITE_ADDR = i;
        }
    }

    int tab[1000];
    for (int i = 0; i < 1000; i++) {
        tab[i] = *READ_ADDR;
    }

    return 0;
}
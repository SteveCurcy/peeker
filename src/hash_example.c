#include <stdio.h>
#include <string.h>

unsigned int str_hash(const char *buf) {
    unsigned int hash = 5381;
    int len = 0;
    for (int i = 0; i < 32; i++) {
        if (buf[i] == '\0') {
            len = i;
            break;
        }
    }
    for (int i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + buf[i]; /* hash * 33 + c */
    }
    return hash;
}
char cmds[9][32] = {0};
char *org[] = {"cat", "touch", "rm", "gzip", "zip", "unzip", "split", "cp", "mv"};

int main() {
    for (int i = 0; i < 9; i++) {
        strcpy(cmds[i], org[i]);
    }
    for (int i = 0; i < 9; i++) {
        printf("%s: %#x\n", cmds[i], str_hash(cmds[i]));
    }
    return 0;
}
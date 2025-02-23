#include <unistd.h>
#include <fcntl.h>

int main() {
    dup2(1, 99);  
    return 0;
}


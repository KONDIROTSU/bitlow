#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_TAPE_SIZE (1024 * 1024 * 8) // Max bits: 1MB tape if needed
#define STACK_SIZE 1024

typedef struct {
    uint8_t *bits;
    size_t size;  // in bits
    size_t pointer;
} Tape;

typedef struct {
    const char *code;
    size_t pc;
    const char *function;
} VM;

Tape tape = {NULL, 0, 0};
VM vm = {0};

void create_tape(size_t bytes) {
    if (tape.bits) free(tape.bits);
    tape.size = bytes * 8;
    tape.bits = calloc((tape.size + 7) / 8, 1); // all bits initialized to 0
    tape.pointer = 0;
}

bool get_bit(size_t index) {
    return (tape.bits[index / 8] >> (7 - (index % 8))) & 1;
}

void set_bit(size_t index, bool val) {
    if (val)
        tape.bits[index / 8] |= (1 << (7 - (index % 8)));
    else
        tape.bits[index / 8] &= ~(1 << (7 - (index % 8)));
}

void set_all(uint8_t val) {
    memset(tape.bits, val ? 0xFF : 0x00, (tape.size + 7) / 8);
}

char read_hex() {
    return vm.code[vm.pc++];
}

uint8_t hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    fprintf(stderr, "Invalid hex character: %c\n", c);
    exit(1);
}

size_t read_hex_bytes(int digits) {
    size_t value = 0;
    for (int i = 0; i < digits; ++i)
        value = (value << 4) | hex_val(read_hex());
    return value;
}

const char *extract_block() {
    int depth = 1;
    size_t start = vm.pc;
    while (vm.code[vm.pc] && depth > 0) {
        char c = vm.code[vm.pc++];
        if (c == '8') depth++;
        else if (c == '9') depth--;
    }
    if (depth != 0) {
        fprintf(stderr, "Unbalanced brackets\n");
        exit(1);
    }
    size_t len = vm.pc - start - 1;
    char *block = malloc(len + 1);
    memcpy(block, vm.code + start, len);
    block[len] = '\0';
    return block;
}

void execute(const char *code); // forward declaration

void run() {
    while (vm.code[vm.pc]) {
        char op = read_hex();
        switch (op) {
            case '0': set_bit(tape.pointer, false); break;
            case '1': set_bit(tape.pointer, true); break;
            case '2': if (tape.pointer > 0) tape.pointer--; break;
            case '3': if (tape.pointer + 1 < tape.size) tape.pointer++; break;
            case '4': set_bit(tape.pointer, !get_bit(tape.pointer)); break;
            case '5': set_all(0); break;
            case '6': tape.pointer = 0; break;
            case '7': tape.pointer = tape.size ? (tape.size - 1) : 0; break;
            case 'A': {
                size_t bytes = read_hex_bytes(2);
                create_tape(bytes);
                break;
            }
            case 'B': {
                size_t count = read_hex_bytes(4);
                if (read_hex() != '8') { fprintf(stderr, "Expected '{'\n"); exit(1); }
                const char *block = extract_block();
                if (count == 0) {
                    while (1) execute(block);
                } else {
                    for (size_t i = 0; i < count; ++i) execute(block);
                }
                free((void *)block);
                break;
            }
            case 'C': {
                if (read_hex() != '8') { fprintf(stderr, "Expected '{'\n"); exit(1); }
                const char *block = extract_block();
                if (get_bit(tape.pointer)) execute(block);
                free((void *)block);
                break;
            }
            case 'D': {
                if (read_hex() != '8') { fprintf(stderr, "Expected '{'\n"); exit(1); }
                const char *block = extract_block();
                if (vm.function) free((void *)vm.function);
                vm.function = block;
                break;
            }
            case 'E':
                if (vm.function) execute(vm.function);
                break;
            case 'F':
                return;
            default:
                fprintf(stderr, "Unknown command: %c\n", op);
                exit(1);
        }
    }
}

void execute(const char *code) {
    size_t prev_pc = vm.pc;
    const char *prev_code = vm.code;
    vm.code = code;
    vm.pc = 0;
    run();
    vm.code = prev_code;
    vm.pc = prev_pc;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s HEXCODE\n", argv[0]);
        return 1;
    }

    clock_t start = clock();

    vm.code = argv[1];
    vm.pc = 0;
    vm.function = NULL;
    run();

    clock_t end = clock();
    double duration_ms = 1000.0 * (end - start) / CLOCKS_PER_SEC;

    // Optional: Print final tape (as bits)
    printf("Tape: ");
    for (size_t i = 0; i < tape.size; ++i)
        printf("%d", get_bit(i));
    printf("\n");

    printf("Execution time: %.3f ms\n", duration_ms);

    free(tape.bits);
    if (vm.function) free((void *)vm.function);
    return 0;
}

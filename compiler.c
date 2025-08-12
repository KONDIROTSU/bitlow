#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_TAPE_SIZE (1024 * 1024 * 8)
#define STACK_SIZE 1024
#define MAX_BLOCK_SIZE 4096

static char block_buffer[MAX_BLOCK_SIZE];

typedef struct {
    uint8_t *bits;
    size_t size;
    size_t pointer;
} Tape;

typedef struct {
    const uint8_t *code_bytes;
    size_t code_size;
    size_t pc;
    const char *function;
} VM;

Tape tape = {NULL, 0, 0};
VM vm = {0};

void set_all(bool val) {
    memset(tape.bits, val ? 0xFF : 0x00, (tape.size + 7) / 8);
}


void create_tape(size_t bytes) {
    size_t new_size = bytes * 8;
    if (tape.bits && new_size <= tape.size) {
        tape.size = new_size;
        tape.pointer = 0;
        set_all(false);
        return;
    }
    free(tape.bits);
    tape.size = new_size;
    tape.bits = calloc((tape.size + 7) / 8, 1);
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

// Read the next nibble as a char
char read_hex() {
    if (vm.pc >= vm.code_size * 2) return '\0'; // End of input
    size_t byte_index = vm.pc / 2;
    bool high_nibble = (vm.pc % 2 == 0);
    uint8_t byte = vm.code_bytes[byte_index];
    vm.pc++;
    uint8_t val = high_nibble ? (byte >> 4) : (byte & 0x0F);
    if (val < 10) return '0' + val;
    else return 'A' + (val - 10);
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
    size_t orig_pc = vm.pc;
    size_t block_len = 0;
    while (vm.pc < vm.code_size * 2 && depth > 0) {
        char c = read_hex();
        if (c == '8') depth++;
        else if (c == '9') depth--;
        if (block_len >= MAX_BLOCK_SIZE) {
            fprintf(stderr, "Block too large\n");
            exit(1);

        }
        block_buffer[block_len++] = c;
    }
    if (depth > 0) {
        fprintf(stderr, "Unmatched block end\n");
        exit(1);
    }
    size_t len = block_len - 1;
    if (len > MAX_BLOCK_SIZE) {
        fprintf(stderr, "Block too large\n");
        exit(1);
    }
    block_buffer[len] = '\0';
    return block_buffer;
}

void execute(const char *code);
void run() {
    while (vm.pc < vm.code_size * 2) {
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
                size_t bytes = read_hex_bytes(4);
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
                //free((void *)block); -- block_buffer, don't free
                break;
            }
            case 'C': {
                if (read_hex() != '8') { fprintf(stderr, "Expected '{'\n"); exit(1); }
                const char *block = extract_block();
                if (get_bit(tape.pointer)) execute(block);
                //free((void *)block); -- block_buffer, don't free
                break;
            }
            case 'D': {
                if (read_hex() != '8') { fprintf(stderr, "Expected '{'\n"); exit(1); }
                const char *block = extract_block();
                if (vm.function) free((void *)vm.function);
                vm.function = strdup(block); // duplicate, we'll free later
                break;
            }
            case 'E':
                if (vm.function) execute(vm.function);
                break;
            case 'F':
                // Will be used for additional commands, like tape saving or rerunning
                return;
            default:
                fprintf(stderr, "Unknown command: %c\n", op);
                exit(1);
        }
    }
}
void execute(const char *code) {
    size_t prev_pc = vm.pc;
    const uint8_t *prev_code_bytes = vm.code_bytes;
    size_t prev_code_size = vm.code_size;

    size_t code_len = strlen(code);
    size_t code_bytes_len = (code_len + 1) / 2;
    uint8_t *code_bytes = malloc(code_bytes_len);
    for (size_t i = 0; i < code_bytes_len; ++i) {
        uint8_t hi = hex_val(code[i * 2]);
        uint8_t lo = (i * 2 + 1 < code_len) ? hex_val(code[i * 2 + 1]) : 0;
        code_bytes[i] = (hi << 4) | lo;
    }

    vm.code_bytes = code_bytes;
    vm.code_size = code_bytes_len;
    vm.pc = 0;
    run();

    free(code_bytes);
    vm.code_bytes = prev_code_bytes;
    vm.code_size = prev_code_size;
    vm.pc = prev_pc;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s CODEFILE\n", argv[0]);
        return 1;
    }
    // Open binary file
    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        perror("Failed to open file");
        return 1;
    }
    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *program = malloc(file_size);
    if (fread(program, 1, file_size, f) != file_size) {
        perror("Failed to read file");
        fclose(f);
        free(program);
        return 1;
    }
    fclose(f);

    clock_t start = clock();
    vm.code_bytes = program;
    vm.code_size = file_size;
    vm.pc = 0;
    vm.function = NULL;
    run();
    clock_t end = clock();
    double duration_ms = 1000.0 * (end - start) / CLOCKS_PER_SEC;
    printf("Tape: ");
    for (size_t i = 0; i < tape.size; ++i)
        printf("%d", get_bit(i));
    printf("\n");
    printf("Execution time: %.3f ms\n", duration_ms);
    free(tape.bits);
    if (vm.function) free((void *)vm.function);
    free(program);
    return 0;
}

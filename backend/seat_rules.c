/*
 * seat_rules.c — Módulo C para validação de assentos
 * 
 * Funções de validação crítica para o sistema de reserva de aviação.
 * Compilar com: gcc -shared -o libseat_rules.so -fPIC seat_rules.c
 */

#include <string.h>
#include <stdio.h>

/**
 * seat_exists — Verifica se um assento existe na lista de válidos.
 * @seat: código do assento (ex: "A1", "B5")
 * @valid_seats: array de códigos válidos
 * @valid_count: tamanho do array
 * 
 * Retorna: 1 se existe, 0 caso contrário
 */
int seat_exists(const char *seat, const char *valid_seats[], int valid_count) {
    if (seat == NULL || valid_seats == NULL) {
        return 0;
    }
    
    for (int i = 0; i < valid_count; i++) {
        if (valid_seats[i] != NULL && strcmp(seat, valid_seats[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * seat_is_reserved — Verifica se um assento já foi reservado.
 * @seat: código do assento
 * @reserved_seats: array de assentos reservados
 * @reserved_count: tamanho do array
 * 
 * Retorna: 1 se reservado, 0 caso contrário
 */
int seat_is_reserved(const char *seat, const char *reserved_seats[], int reserved_count) {
    if (seat == NULL || reserved_seats == NULL) {
        return 0;
    }
    
    for (int i = 0; i < reserved_count; i++) {
        if (reserved_seats[i] != NULL && strcmp(seat, reserved_seats[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * can_reserve_seat — Valida se um assento pode ser reservado.
 * 
 * Regras:
 * - O assento deve existir na aeronave
 * - O assento não deve estar já reservado
 * 
 * @seat: código do assento a reservar
 * @valid_seats: array de assentos válidos da aeronave
 * @valid_count: tamanho do array de válidos
 * @reserved_seats: array de assentos já reservados
 * @reserved_count: tamanho do array de reservados
 * 
 * Retorna: 1 se pode reservar, 0 caso contrário
 */
int can_reserve_seat(
    const char *seat,
    const char *valid_seats[],
    int valid_count,
    const char *reserved_seats[],
    int reserved_count
) {
    /* Validar entrada */
    if (seat == NULL) {
        return 0;
    }
    
    /* Verificar se o assento existe */
    if (!seat_exists(seat, valid_seats, valid_count)) {
        return 0;
    }
    
    /* Verificar se o assento já está reservado */
    if (seat_is_reserved(seat, reserved_seats, reserved_count)) {
        return 0;
    }
    
    /* Assento está disponível e válido */
    return 1;
}

/**
 * validate_seat_range — Valida se um intervalo de assentos é válido.
 * Útil para futuras funcionalidades de reserva em lote.
 * 
 * @start_seat: assento inicial (ex: "A1")
 * @end_seat: assento final (ex: "A5")
 * @valid_seats: array de assentos válidos
 * @valid_count: tamanho do array
 * 
 * Retorna: 1 se intervalo é válido, 0 caso contrário
 */
int validate_seat_range(
    const char *start_seat,
    const char *end_seat,
    const char *valid_seats[],
    int valid_count
) {
    if (start_seat == NULL || end_seat == NULL) {
        return 0;
    }
    
    return seat_exists(start_seat, valid_seats, valid_count) &&
           seat_exists(end_seat, valid_seats, valid_count);
}

/**
 * count_available_seats — Conta quantos assentos estão disponíveis.
 * 
 * @valid_seats: array de assentos válidos
 * @valid_count: tamanho do array
 * @reserved_seats: array de assentos reservados
 * @reserved_count: tamanho do array
 * 
 * Retorna: número de assentos disponíveis
 */
int count_available_seats(
    const char *valid_seats[],
    int valid_count,
    const char *reserved_seats[],
    int reserved_count
) {
    int available = valid_count;
    
    for (int i = 0; i < reserved_count; i++) {
        if (reserved_seats[i] != NULL && seat_exists(reserved_seats[i], valid_seats, valid_count)) {
            available--;
        }
    }
    
    return available;
}

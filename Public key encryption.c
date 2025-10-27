// rsa_demo.c
// 간단한 RSA 데모 (교육용)
// 컴파일: gcc -std=c11 -O2 -o rsa_demo rsa_demo.c

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>

// --- 유틸리티: 큰 곱셈 및 모듈러 거듭제곱 (128-bit 사용) ---
static inline uint64_t mod_mul(uint64_t a, uint64_t b, uint64_t mod) {
    __int128 res = (__int128)a * (__int128)b;
    res %= mod;
    return (uint64_t)res;
}

uint64_t mod_pow(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t res = 1 % mod;
    uint64_t b = base % mod;
    while (exp) {
        if (exp & 1) res = mod_mul(res, b, mod);
        b = mod_mul(b, b, mod);
        exp >>= 1;
    }
    return res;
}

// --- 확장 유클리드: ax + by = g, (g = gcd(a,b)) ---
int64_t egcd(int64_t a, int64_t b, int64_t *x, int64_t *y) {
    if (b == 0) { *x = 1; *y = 0; return a; }
    int64_t x1, y1;
    int64_t g = egcd(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (a / b) * y1;
    return g;
}

// 모듈러 역원 (a * inv ≡ 1 (mod m)), 실패 시 0 반환 (역원 없음)
uint64_t modinv(uint64_t a, uint64_t m) {
    int64_t x, y;
    int64_t g = egcd((int64_t)a, (int64_t)m, &x, &y);
    if (g != 1) return 0; // 역원 없음
    int64_t inv = x % (int64_t)m;
    if (inv < 0) inv += m;
    return (uint64_t)inv;
}

// 최대공약수
uint64_t gcd_u64(uint64_t a, uint64_t b) {
    while (b) { uint64_t t = a % b; a = b; b = t; }
    return a;
}

// --- 작은 소수 목록 (교육용) ---
const uint32_t small_primes[] = {
    1019, 1151, 1229, 1277, 1289, 1301, 1409, 1427, 1439, 1459,
    1481, 1499, 1511, 1523, 1531, 1559, 1583, 1601, 1613, 1627,
    1657, 1663, 1693, 1709, 1723, 1741, 1747, 1759, 1783, 1801,
    1811, 1823, 1831, 1847, 1861, 1871, 1877, 1889, 1901, 1907,
    1913, 1931, 1949, 1951, 1973, 1993, 1997, 1999, 2003, 2011
};
const size_t PRIMES_COUNT = sizeof(small_primes) / sizeof(small_primes[0]);

// 랜덤하게 두 소수 선택 (겹치지 않게)
void pick_two_primes(uint64_t *p, uint64_t *q) {
    srand((unsigned)time(NULL) ^ (unsigned)getpid());
    size_t i = rand() % PRIMES_COUNT;
    size_t j;
    do { j = rand() % PRIMES_COUNT; } while (j == i);
    *p = small_primes[i];
    *q = small_primes[j];
}

// 키 생성: (n, e) 공개키, (n, d) 개인키
// e는 보통 65537을 시도 (여기서는 65537이 gcd(phi, e)=1인지 확인)
int generate_rsa_keys(uint64_t *n, uint64_t *e, uint64_t *d) {
    uint64_t p, q;
    pick_two_primes(&p, &q);
    *n = p * q;
    uint64_t phi = (p - 1) * (q - 1);

    uint64_t chosen_e = 65537;
    if (chosen_e >= phi || gcd_u64(chosen_e, phi) != 1) {
        // 65537이 안되면 작은 소수들 중에서 골라본다
        const uint32_t small_es[] = {3,5,17,257};
        size_t k;
        int found = 0;
        for (k = 0; k < sizeof(small_es)/sizeof(small_es[0]); ++k) {
            if (gcd_u64(small_es[k], phi) == 1) { chosen_e = small_es[k]; found = 1; break; }
        }
        if (!found) return 0; // 실패 (이 데모에선 거의 일어나지 않음)
    }

    uint64_t chosen_d = modinv(chosen_e, phi);
    if (chosen_d == 0) return 0;

    *e = chosen_e;
    *d = chosen_d;
    return 1;
}

// 문자열을 한 문자씩 암호화/복호화 (교육용, 블록 패딩 없음)
void encrypt_message(const char *msg, uint64_t e, uint64_t n, uint64_t *out, size_t *out_len) {
    size_t L = strlen(msg);
    for (size_t i = 0; i < L; ++i) {
        uint64_t m = (uint8_t)msg[i];
        out[i] = mod_pow(m, e, n);
    }
    *out_len = L;
}

void decrypt_message(const uint64_t *ct, size_t ct_len, uint64_t d, uint64_t n, char *out) {
    for (size_t i = 0; i < ct_len; ++i) {
        uint64_t m = mod_pow(ct[i], d, n);
        out[i] = (char)(m & 0xFF);
    }
    out[ct_len] = '\0';
}

int main(void) {
    printf("=== 간단한 RSA 데모 (교육용) ===\n\n");

    uint64_t n, e, d;
    if (!generate_rsa_keys(&n, &e, &d)) {
        fprintf(stderr, "키 생성 실패\n");
        return 1;
    }

    printf("생성된 키 (교육용, 작고 안전하지 않음)\n");
    printf("공개키: n = %" PRIu64 ", e = %" PRIu64 "\n", n, e);
    printf("개인키: d = %" PRIu64 "\n\n", d);

    char plaintext[1024];
    printf("암호화할 문자열을 입력하세요 (최대 1023자):\n> ");
    if (!fgets(plaintext, sizeof(plaintext), stdin)) return 0;
    // fgets의 끝 \n 제거
    size_t L = strlen(plaintext);
    if (L > 0 && plaintext[L-1] == '\n') plaintext[L-1] = '\0';

    // 암호화
    uint64_t ciphertext[1024];
    size_t ct_len = 0;
    encrypt_message(plaintext, e, n, ciphertext, &ct_len);

    printf("\n암호문 (각 숫자는 하나의 바이트를 암호화한 결과):\n");
    for (size_t i = 0; i < ct_len; ++i) {
        printf("%" PRIu64 " ", ciphertext[i]);
        if ((i+1) % 16 == 0) printf("\n");
    }
    printf("\n\n");

    // 복호화
    char recovered[1024];
    decrypt_message(ciphertext, ct_len, d, n, recovered);
    printf("복호화 결과:\n%s\n", recovered);

    printf("\n--- 주의 ---\n");
    printf("1) 이 구현은 교육 및 실습용입니다. 실제 보안 목적에 사용하지 마세요.\n");
    printf("2) 실제 환경에서는 큰 소수(수백 ~ 수천 비트)를 사용하고, 신뢰된 암호 라이브러리를 사용하세요.\n");

    return 0;
}

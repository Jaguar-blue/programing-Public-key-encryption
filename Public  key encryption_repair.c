#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h> 

int is_prime(int x) {   //gpt코드 이용 
	int i;
    if (x < 2) return 0;
    for (i = 2; i * i <= x; i++) {
        if (x % i == 0) return 0;
    }
    return 1;
}

/* 최대공약수 (유클리드) */
long long gcd(long long a, long long b) {   //gpt코드 이용 
    while (b != 0) {
        long long t = a % b;
        a = b;
        b = t;
    }
    return a;
}

/* 확장 유클리드: a*s + b*t = gcd(a,b)         <<< e,pi넣을 건데, e,p서로소 이므로 gcd(a,b)=1, a*s + b*t = 1 만족시키는 d=s찾는것 
   d = s (모듈러 역원) 구하는 데 사용 */
long long modinv(long long a, long long m) {   //gpt코드 이용
    long long t = 0, newt = 1;
    long long r = m, newr = a;
    while (newr != 0) {
        long long q = r / newr;
        long long tmp = newt;
        newt = t - q * newt; t = tmp;
        tmp = newr;
        newr = r - q * newr; r = tmp;
    }
    if (r > 1) return -1; // 역원 없음
    if (t < 0) t += m;
    return t;
}

long long modpow(long long a, long long b, long long n) {   //모듈러 거듭제곱->나머지의 거듭제곱 -> a%n=a, (a*b)%n = ((a%n)*(b%n))%n 이용 
	long long i, r=1;
	for(i=0;i<b;i++) {
		r=(r*a)%n;
	}
	return r; 
}
int main()
{
	long long p, q, n, pi, e, d, msg, cipher, decryp;
	srand((unsigned)time( NULL ));
	printf("송신할 메시지 입력(한 문자) : \n");
	scanf("%c", &msg);
	printf("%d\n", msg);
	//printf("2^3=%d\n", 2^3); -> C에서 ^안됨
	//printf("2^3=%.0lf\n", pow(2, 3));
	printf("2^3=%lld", modpow(2, 3, 5));
	printf("\n----------\n");
	
	printf("수신자가 두소수 p,q를 생성한다.\n");
	p=rand();
	q=rand();
	while(is_prime(p)==0) p=rand()+20;
	while(is_prime(q)==0) q=rand()+20;
	printf("수신자가 n=p*q를 생성하고 전송한다.\n");
	printf("수신자가 pi=(p-1)*(q-1)를 생성한다.\n");
	n=p*q;
	pi=(p-1)*(q-1);
	printf("수신자가 pi와 서로소이고 pi보다 작은 수 e를 생성하고 전송한다.\n");
	e=rand();
	while(gcd(e, pi)!=1 || e>=pi) {
		e=rand();
	}
	printf("수신자가 e*d %% pi==1을 만족시키는 d를 생성한다.\n");
	d = modinv(e, pi); //챝gpt 코드 사용
    if (d == -1) {
        printf("개인지수(d) 생성 실패\n");
        return 1;
    }
	printf(">\n");
	printf("((( p, q : %d, %d ))) (수신자만 앎)\n", p, q);
	printf("n : %d (공개됨)\n", n);
	printf("((( pi : %d ))) (수신자만 앎)\n", pi);
	printf("e : %d (공개됨)\n", e);
	printf("((( d : %d ))) (수신자만 앎)\n", d);
	
	printf("\n");
	printf("송신자가 암호문=(msg^e)%n으로 메세지를 암호화한다.\n");
	cipher = modpow(msg, e, n);
	printf("송신자가 암호문을 전송한다.\n");
	printf(">\n");
	printf("암호문 : %lld (공개됨)\n", cipher);
	
	printf("\n");
     
    printf("수신자가 복호문=(암호문^d)%n으로 메세지를 복호화한다.\n");
    decryp = modpow(cipher, d, n); 
	printf(">\n");
	printf("((( 복호문 : %d ))) (수신자만 앎)\n", decryp);
	printf("\n----------\n");
	printf("%c\n", (char)decryp);
	
	return 0;
}

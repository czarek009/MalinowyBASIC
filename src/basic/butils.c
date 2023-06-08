#include "butils.h"
#include "printf.h"


bool isalpha(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}
bool isdigit(char c) {
  return c >= '0' && c <= '9';
}
bool isalphanum(char c) {
  return isalpha(c) || isdigit(c);
}

size_t strlen(char* s) {
  size_t out = 0;
  while (s[out] != '\0') {
    ++out;
  }

  return out;
}

void strncpy(char *dest, const char *src, size_t n) {
  for(int i=0; i<n; ++i) {
    // if (src[i] == 0) return;
    dest[i] = src[i];
  }
}

int strncmp(const char *src1, const char *src2, register size_t n) {
  register unsigned char u1, u2;

  while (n-- > 0) {
    u1 = (unsigned char) *src1++;
    u2 = (unsigned char) *src2++;
    if (u1 != u2)
      return u1 - u2;
    if (u1 == '\0')
      return 0;
  }
  return 0;
}

s64 str2s64(char* str) {
  s64 out = 0;
  int i = 0;
  if (str[0] == '-' || str[0] == '+') {
    ++i;
  }
  for (; isdigit(str[i]); ++i) {
    out *= 10;
    out += (u8)(str[i] - 48);
  }

  if (str[0] == '-') {
    out *= -1;
  }

  return out;
}
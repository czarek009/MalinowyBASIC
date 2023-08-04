#include "butils.h"


bool isalpha(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}
bool isdigit(char c) {
  return c >= '0' && c <= '9';
}
bool isalphanum(char c) {
  return isalpha(c) || isdigit(c);
}

bool isin(char c, char* p) {
  for (int i = 0; p[i] != '\0'; ++i) {
    if (c == p[i])
      return true;
  }
  return false;
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

bool non_empty(char* str) {
  u64 i = 0;
  if (str[0] == '\0') {
    return false;
  }
  while(str[i] != '\0') {
    if (str[i] != ' ') {
      return true;
    }
  }
  return false;
}

u8 is_valid_varname(char* cmd) {
  /*
   * Check if a valid variable name starts at *cmd
   *
   * Args:
   *   cmd - sting that contains a basic command
   *
   * Return:
   *   If *cmd contains first character of a valid variable name, return its length
   *   returns 0 otherwise
   */
  if (!isalpha(*cmd)) {
    // must start with a letter
    return 0;
  }
  for (int i=1; i<7; ++i) {
    if (!isalphanum(cmd[i])) {
      // if non alphanum character occurs then varname lenght is less than 7, return it
      return i;
    }
  }
  if (isalphanum(cmd[7])) {
    // varname too long
    return 0;
  }
  // valid varname of max length (eq 7)
  return 7;
}

u8 strcmp(char *str1, char *str2) {
  u8 i = 0;
  for(; str1[i] != '\0' && str2[i] != '\0'; i++) {
    if(str1[i] != str2[i]){
      return 1;
    }
  }
  if(str1[i] == '\0' && str2[i] == '\0'){
    return 0;
  }
  return 1;
}

/* 
 * Find str1 in str2.
 * Return starting index if str1 is in str2.
 * Otherwise return strlen(str2);
 */
u64 find_substring(char* str1, char* str2) {

  for (u64 i = 0; i < strlen(str2); ++i) {
    bool found = true;
    for (u64 j = 0; j < strlen(str1); ++j) {
      if (str2[i+j] != str1[j]) {
        found = false;
        break;
      }
    }
    if (found) {
      return i;
    }
  }

  return strlen(str2);
}

u64 find_last_substring(char* str1, char* str2) {
  size_t len1 = strlen(str1) - 1;
  size_t len2 = strlen(str2) - 1;
  for (s64 i = len2; 0 <= i; i--) {
    bool found = true;
    for (s64 j = len1; 0 < j; j--) {
      if (str2[i+j] != str1[j]) {
        found = false;
        break;
      }
    }
    if (found) {
      return i;
    }
  }
  return len2;
}
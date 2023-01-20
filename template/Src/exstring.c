#include "exstring.h"

//---------------------------------------------------------------------------------------------------------------------

char *substr(char *string, int16_t start, int16_t length)
{
  int16_t size = strlen(string);
  if(start < 0) start += size;
  if(length <= 0) length = length + size - start;
  char *string_out = malloc(length + 1);
  memcpy(string_out, &string[start], length);
  string_out[length] = 0;
  return string_out;
}

char *joinstr(unsigned int count, const char *str, ...)
{
  unsigned int i;
  va_list args;
  size_t size = 1, sizeary[count];

  va_start(args, str);
  char *tmp = (char *)str;
  for(i = 0; i < count; i++)
  {
    sizeary[i] = strlen(tmp);
    size += sizeary[i];
    tmp = va_arg(args, char *);
  }
  va_end(args);

  char *strout = malloc(size);
  strout[size-1] = 0;

  char *marker = strout;
  va_start(args, str);
  tmp = (char *)str;
  for(i = 0; i < count; i++)
  {
    memcpy(marker, tmp, sizeary[i]);
    marker += sizeary[i];
    tmp = va_arg(args, char*);
  }
  va_end(args);
  return strout;
}

char *strcopy(char *str)
{
  size_t size = strlen(str) + 1;
  char *cp = malloc(size);
  memcpy(cp, str, size);
  return cp;
}

//---------------------------------------------------------------------------------------------------------------------

const char LowerChars[256] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0xAB, 0x8E, 0x86,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x98, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
  0xA0, 0xA1, 0xA2, 0xA3, 0xA5, 0xA5, 0xA6, 0xA7, 0xA9, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
  0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBE, 0xBE, 0xBF,
  0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
  0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0x88, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
  0xA2, 0xE1, 0xE2, 0xE4, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
  0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

const char UpperChars[256] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
  0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x8F, 0x87, 0x9D, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x97, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
  0xA0, 0xA1, 0xE0, 0xA3, 0xA4, 0xA4, 0xA6, 0xA7, 0xA8, 0xA8, 0xAA, 0x8D, 0xAC, 0xAD, 0xAE, 0xAF,
  0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBD, 0xBF,
  0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
  0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
  0xE0, 0xE1, 0xE2, 0xE3, 0xE3, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
  0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

char *strtolower_this(char *str)
{
  char *p = str;
  while(*p) {
    *p = LowerChars[(uint8_t)*p];
    p++;
  }
  return str;
}

char *strtolower(char *str)
{
  char *cp = strcopy(str);
  return strtolower_this(cp);
}

char *strtoupper_this(char *str)
{
  char *p = str;
  while(*p) {
    *p = UpperChars[(uint8_t)*p];
    p++;
  }
  return str;
}

char *strtoupper(char *str)
{
  char *cp = strcopy(str);
  return strtoupper_this(cp);
}

//--------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief Funkcja konwertuje zmienne liczbowe z typu całkowitego ze znakiem na łańcuch znaków (charów).
 * @param int32_t nbr:        Liczba do konwersji.
 * @param uint8_t* str:       Wyjściowy łańcuch znaków (odwrócony),
 *                            tnz. musi być ładowany do bufora od końca [n-1] do początku [0].
 * @param uint8_t base:       Podstawa systemu liczbowego.
 * @param uint8_t fill_zero:  Wypełnienie zerami (fill_zero >= 1).
 * @param uint8_t fill_space: Wypełnienie spacjami (fill_space >= fill_zero).
 * @retval uint8_t:           Liczba znaczących znaków w ciągu 'str'.
 */
uint8_t itoa_base(int32_t nbr, char *str, uint8_t base, bool sign, uint8_t fill_zero, uint8_t fill_space)
{
  const uint8_t ITOA_ARRAY[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  uint16_t n = 0;
  bool is_negative = false;
  uint32_t unbr;

  if(!fill_zero) fill_zero = 1;
  if(sign && nbr < 0)
  {
    is_negative = true;
    unbr = (uint32_t)(-nbr);
  }
  else unbr = (uint32_t)nbr;

  while (unbr != 0)
  {
    str[n++] = ITOA_ARRAY[unbr % base];
    unbr = unbr/base;
  }

  while(n < fill_zero) str[n++] = '0';
  if(is_negative) str[n++] = '-';
  while(n < fill_space) str[n++] = ' ';

  return n;
}

char *itoa_int(int32_t nbr, uint8_t base, bool sign, uint8_t fill_zero, uint8_t fill_spac)
{
  char temp[32];
  uint8_t len = itoa_base(nbr, temp, base, sign, fill_zero, fill_spac);
  uint8_t inc = 0;
  char *string = malloc(len + 1);

  string[len] = 0;
  while(len) string[inc++] = temp[--len];
  return string;
}

/**
 * @brief Funkcje konwertuje zmienne liczbowe typu całkowitego na łańcuch string w odpowiednim formacie.
 * @param int32_t nbr: Liczba do konwersji.
 * @retval char *: Wskaźnik na wynikowy łańcuch string
 */
char *itoa_dec(int32_t nbr) { return itoa_int(nbr, 10, true, 0, 0); }
char *itoa_udec(uint32_t nbr) { return itoa_int((int32_t)nbr, 10, false, 0, 0); }
char *itoa_hex8(uint32_t nbr) { return itoa_int((int32_t)nbr, 16, false, 2, 2); }
char *itoa_hex16(uint32_t nbr) { return itoa_int((int32_t)nbr, 16, false, 4, 4); }
char *itoa_hex32(uint32_t nbr) { return itoa_int((int32_t)nbr, 16, false, 8, 8); }

char *itoa_array(uint8_t *ary, uint16_t len)
{
  uint16_t i;
  char *string = malloc((2 * len) + 1);
  char str_value[2];

  for(i = 0; i < len; i++) {
    uint8_t value = ary[i];
    itoa_base(value, str_value, 16, false, 2, 2);
    string[2 * i] = str_value[1];
    string[2 * i + 1] = str_value[0];
  }
  string[2 * i] = 0;

  return string;
};

double uatof(char *str)
{
  double value = atof(str);

  while((*str >= '0' && *str <= '9') || *str == '-' || *str == '.') str++;

  switch(*str)
  {
    case 'g': value *= 1000000000; break;
    case 'm':
      if(*(str + 1) == 'e' && *(str + 2) == 'g') value *= 1000000;
      else value /= 1000;
      break;
    case 'k': value *= 1000; break;
    case '%': value /= 100; break;
    case 'u': value /= 1000000; break;
    case 'n': value /= 1000000000; break;
  }
  return value;
}

//--------------------------------------------------------------------------------------------------------------------------------

char *reverse_this_string(char *str)
{
  char *begin, *end, box;
  size_t size = strlen(str);

  begin = str;
  end = str;

  if(size) {
    for(size_t i = 0; i < size - 1; i++) end++;
    for (size_t i = 0; i < size / 2; i++) {
      box = *end;
      *end = *begin;
      *begin = box;
      begin++;
        end--;
    }
  }
  return str;
}

char *reverse_string(char *str_in)
{
  size_t size = strlen(str_in);
  char *str_out = malloc(size + 1);
  memcpy(str_out, str_in, size + 1);
  reverse_this_string(str_out);
  return str_out;
}

void reverse_this_array8(uint8_t *array, uint16_t length)
{
  uint16_t start = 0;
  uint16_t end = length;

  while(start < end) {
    int temp = array[start];
    array[start] = array[end];
    array[end] = temp;
    start++;
    end--;
  }
}

uint8_t *reverse_array8(uint8_t *array, uint16_t length)
{
  uint8_t *malloc_array = (uint8_t *)malloc(length);
  memcpy(malloc_array, array, length);
  reverse_this_array8(malloc_array, length);
  return malloc_array;
}

int find(char *str, char *pattern)
{
  uint8_t marker = 0;
  int pointer = 1;

  while(*str) {
    if(*str == pattern[marker]) {
      marker++;
      if(!pattern[marker]) return pointer;
    }
    else marker = 0;
    pointer++;
    str++;
  }
  return 0;
}

int find_right(char *str, char *pattern)
{
  char *reverse_str = reverse_string(str);
  char *reverse_pattern = reverse_string(pattern);
  return find(reverse_str, reverse_pattern);
}

int find_in_array(uint8_t *array, uint16_t length, char *pattern)
{
  uint8_t marker = 0;
  int pointer = 0;

  while(length) {
    if(array[pointer] == pattern[marker]) {
      marker++;
      if(!pattern[marker]) return pointer + 1;
    }
    else marker = 0;
    pointer++;
    length--;
  }
  return 0;
}

char *cut_array_this(char *str, char *pattern, CUT_e option)
{
  int x;
  switch(option) {
    case CUT_StartLeft_TakeLeft:
      x = find(str, pattern);
      if(x) str[find(str, pattern) - strlen(pattern)] = 0;
      break;
    case CUT_StartRight_TakeLeft:
      x = find_right(str, pattern);
      if(x) str[strlen(str) - x] = 0;
      break;
    case CUT_StartLeft_TakeRight:
      x = find(str, pattern);
      if(x) str = &str[x];
      break;
    case CUT_StartRight_TakeRight:
      x = find_right(str, pattern);
      if(x) str = &str[strlen(str) - x + strlen(pattern)];
      break;
    default:
      break;
  }
  return str;
}

char *cut_this(char *str, char *pattern, CUT_e option)
{
  int x;
  switch(option) {
    case CUT_StartLeft_TakeLeft:
      x = find(str, pattern);
      if(x) str[find(str, pattern) - strlen(pattern)] = 0;
      break;
    case CUT_StartRight_TakeLeft:
      x = find_right(str, pattern);
      if(x) str[strlen(str) - x] = 0;
      break;
    case CUT_StartLeft_TakeRight:
      x = find(str, pattern);
      if(x) str = &str[x];
      break;
    case CUT_StartRight_TakeRight:
      x = find_right(str, pattern);
      if(x) str = &str[strlen(str) - x + strlen(pattern)];
      break;
    default:
      break;
  }
  return str;
}

char *cut(char *str_in, char *pattern, CUT_e option)
{
  size_t size = strlen(str_in);
  char *str_out = malloc(size + 1);
  memcpy(str_out, str_in, size + 1);
  str_out = cut_this(str_out, pattern, option);
  return str_out;
}

static int _find_char(char byte, char *list)
{
  int pointer = 1;
  while(*list)
  {
    if(byte == *list) return pointer;
    pointer++;
    list++;
  }
  return 0;
}

char *ltrim_list(char *str, char *list)
{
  while(_find_char(*str, list)) str++;
  return str;
}

char *ltrim(char *str)
{
  while(isspace((int)*str)) str++;
  return str;
}

char *rtrim_list(char *str, char *list)
{
  char* back = str + strlen(str);
  while(_find_char(*--back, list));
  *(back+1) = '\0';
  return str;
}

char *rtrim(char *str)
{
  char* back = str + strlen(str);
  while(isspace((int)*--back));
  *(back+1) = '\0';
  return str;
}

char *trim_list(char *str, char *list)
{
  return ltrim_list(rtrim_list(str, list), list);
}

char *trim(char *str)
{
  return ltrim(rtrim(str));
}

//--------------------------------------------------------------------------------------------------------------------------------

uint16_t char_count(char *str, char val)
{
  uint16_t count = 0;
  while(*str) {
    if(*str == val) count++;
    str++;
  }
  return count;
}

//--------------------------------------------------------------------------------------------------------------------------------

/* Funkcja wyciąga dany łańcuch znaków pomiędzy wskazanymi znakami separującymi.
 *
 * @param *str:        Wejściowy łańcuch znaków.
 * @param position:    Wskazuje, który z kolei łańcuch znaków ma zostać zwrócony.
 * @param delimiter:   Znak, który jest separatorem.
 *
 * @retval:            Fragment łańcuch znaków, o który prosiliśmy.
 */

char *extraction(char *str, char delimiter, int position)
{
  size_t size = 0;
  char *pointner;
  uint8_t counter_start = 0;

  if(!position) {
    pointner = str;
    counter_start = 1;
  }

  while(*str)
  {
    if(*str == delimiter) {
      if(position) position--;
      else break;
    }
    str++;
    if(!position)
    {
      if(counter_start) size++;
      else
      {
        pointner = str;
        counter_start = 1;
      }
    }
  }

  char *str_out = malloc(size + 1);
  if(size) memcpy(str_out, pointner, size);
  str_out[size] = 0;
  return str_out;
}

int explode(char ***arr_ptr, char *str, char delimiter)
{
  char *src = str, *end, *dst;
  char **arr;
  int size = 1, i;

  while((end = strchr(src, delimiter)) != NULL) { ++size; src = end + 1; }
  arr = malloc(size * sizeof(char *) + (strlen(str) + 1) * sizeof(char));

	src = str;
	dst = (char *)arr + size * sizeof(char *);
	for(i = 0; i < size; ++i)
	{
		if((end = strchr(src, delimiter)) == NULL) end = src + strlen(src);
		arr[i] = dst;
		strncpy(dst, src, end - src);
		dst[end - src] = '\0';
		dst += end - src + 1;
		src = end + 1;
	}
	*arr_ptr = arr;
  return size;
}

//--------------------------------------------------------------------------------------------------------------------------------

char *replace_this_char(char *pattern, char replacement, char *str)
{
  char *pointner;

  while(*pattern)
  {
    pointner = strchr(str, *pattern);
    if(*pattern != replacement)
    {
      while(pointner)
      {
        *pointner = replacement;
        pointner = strchr(pointner, *pattern);
      }
    }
    pattern++;
  }
  return str;
}

char *replace_char(char *pattern, char replacement, char *original)
{
  size_t size = strlen(original) + 1;
  char *str = malloc(size);
  memcpy(str, original, size);

  return replace_this_char(pattern, replacement, str);
}

/* Funkcja podmienia wybrane znaki w danym łańcuchu.
 *
 * @param *pattern:    Lista znaków (jako string), które należy podmienić.
 * @param replacement: Znak, którym mają zostać zastąpione znaki z listy.
 * @param *original:   Łańcuch znaków, w którym mają zostać podmienione znaki.
 *
 * @retval:            Łańcuch znaków z podmienionymi znakami
 */

/*

char *replace_string(char *pattern, char *replacement, char *original)
{
	size_t const replen = strlen(replacement);
	size_t const patlen = strlen(pattern);
	size_t const orilen = strlen(original);

	size_t patcnt = 0;
	const char *oriptr;
	const char *patloc;

	// find how many times the pattern occurs in the original string
	for(oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen)
		patcnt++;

	size_t const retlen = orilen + patcnt * (replen - patlen);
	char *const returned = malloc(sizeof(char) * (retlen + 1));

	if(returned != NULL)
	{
		// copy the original string,
		// replacing all the instances of the pattern
		char *retptr = returned;
		for(oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen)
		{
			size_t const skplen = patloc - oriptr;
			// copy the section until the occurence of the pattern
			strncpy(retptr, oriptr, skplen);
			retptr += skplen;
			// copy the replacement
			strncpy(retptr, replacement, replen);
			retptr += replen;
		}
		// copy the rest of the string.
		strcpy(retptr, oriptr);
	}
	return returned;
}

*/

//------------------------------------------------------------------------------------------------

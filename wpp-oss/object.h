#ifndef OBJECT_H
#define OBJECT_H

enum
{
  WPP_OBJ_INT,
  WPP_OBJ_FLOAT,
  WPP_OBJ_STRING,
  WPP_OBJ_FUNCTION
};

typedef struct
{
  unsigned char type;

  union
  {
    struct
    {
      unsigned int pos;
    } function;

    int _int;
    float _float;
    char *string;
  } as;

  char *name;
} wppObject;

#endif

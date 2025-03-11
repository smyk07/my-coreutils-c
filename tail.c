#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct arguments {
  unsigned int file_count;
  char **files;
} arguments;

void parse_arguments(int argc, char *argv[], arguments *args) {
  args->files = malloc(argc * sizeof(char));

  if (args->files == NULL) {
    perror("malloc failed");
    exit(1);
  }

  int index = 0;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--help") == 0) {
      printf("Usage: ./tail [FILES] [--help]\n");
      printf("Output the last 10 lines of a FILE.\n");
      printf("If more than one FILE, preceed each with a header displaying the "
             "filename.\n");
      free(args->files);
      exit(0);
    } else {
      args->files[index] = argv[i];
      index += 1;
    }
  }

  args->file_count = index;
}

#define MAX_LEN 4096
#define MAX_LINES 10

int read_stdin_end(char **buffer) {
  int tmp_capacity = MAX_LEN;
  char *tmp = malloc(tmp_capacity * sizeof(char));

  if (tmp == NULL) {
    perror("malloc failed");
    exit(1);
  }

  int tmp_size = 0;
  int size = 0;

  while ((size = fread(tmp + tmp_size, sizeof(char), MAX_LEN, stdin)) > 0) {
    tmp_size += size;
    if (tmp_size + MAX_LEN >= tmp_capacity) {
      tmp_capacity *= 2;
      tmp = realloc(tmp, tmp_capacity * sizeof(char));

      if (tmp == NULL) {
        perror("realloc failed");
        exit(1);
      }
    }
  }

  int lines_count = 0;

  for (int i = tmp_size - 1; i >= 0; i--) {
    if (tmp[i] == '\n') {
      lines_count++;
      if (lines_count == 11) {
        *buffer = strdup(tmp + i + 1);
        free(tmp);
        return strlen(*buffer);
      }
    }
  }

  *buffer = strdup(tmp);
  free(tmp);
  return strlen(*buffer);
}

int read_file_end(char *path, char **buffer) {
  int tmp_capacity = MAX_LEN;
  char *tmp = malloc(tmp_capacity * sizeof(char));

  if (tmp == NULL) {
    perror("malloc failed");
    exit(1);
  }

  int tmp_size = 0;

  FILE *f = fopen(path, "r");

  if (f == NULL) {
    perror("fopen failed");
    free(tmp);
    exit(1);
  }

  fseek(f, 0, SEEK_END);
  long file_size = ftell(f);
  long i = file_size - 1;

  int newline_count = 0;
  char ch;

  for (; i >= 0; i--) {
    fseek(f, i, SEEK_SET);
    ch = fgetc(f);

    if (ch == '\n') {
      newline_count++;
      if (newline_count == 11) {
        i++;
        break;
      }
    }
  }

  if (i < 11) {
    rewind(f);
  } else {
    fseek(f, i, SEEK_SET);
  }

  while ((ch = fgetc(f)) != EOF) {
    if (tmp_size >= tmp_capacity - 1) {
      tmp_capacity *= 2;
      tmp = realloc(tmp, tmp_capacity);
      if (tmp == NULL) {
        perror("realloc failed");
        fclose(f);
        exit(1);
      }
    }

    tmp[tmp_size++] = ch;
  }

  tmp[tmp_size] = '\0';

  fclose(f);

  *buffer = tmp;

  return tmp_size;
}

int main(int argc, char *argv[]) {
  arguments args = {0};
  parse_arguments(argc, argv, &args);

  if (args.file_count == 0) {
    char *content = NULL;
    int size = read_stdin_end(&content);

    if (size > 0) {
      fwrite(content, sizeof(char), size, stdout);
    }

    free(content);
  } else if (args.file_count == 1) {
    char *buffer = NULL;
    int buffer_size = read_file_end(args.files[0], &buffer);
    buffer = realloc(buffer, buffer_size);
    if (buffer == NULL) {
      perror("realloc failed");
      free(buffer);
      free(args.files);
      exit(1);
    }

    if (buffer_size > 0) {
      fwrite(buffer, sizeof(char), buffer_size, stdout);
    }

    free(buffer);
  } else {
    for (int i = 0; i < args.file_count; i++) {
      char *content = NULL;
      int size = read_file_end(args.files[i], &content);

      if (size > 0) {
        printf("==> %s <==\n", args.files[i]);
        fwrite(content, sizeof(char), size, stdout);
      }

      free(content);
    }
  }

  free(args.files);

  return 0;
}

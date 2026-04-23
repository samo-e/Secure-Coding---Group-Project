#include <stdio.h>
#include <stdlib.h>

#include "bun.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <file.bun>\n", argv[0]);
    return BUN_ERR_IO;
  }
  const char *path = argv[1];

  BunParseContext ctx = {0};
  BunHeader header = {0};

  bun_result_t result = bun_open(path, &ctx);
  if (result != BUN_OK) {
    fprintf(stderr, "Error: could not open '%s'\n", path);
    return result;
  }

  result = bun_parse_header(&ctx, &header);
  if (result != BUN_OK) {
    // bun_parse_header returns a code; printing the specifics is up to
    // you -- you may want to extend the API to return error details
    fprintf(stderr, "Error: header invalid or unsupported (code %d)\n", result);
    bun_close(&ctx);
    return result;
  }

  printf("BUN header:\n"
         "  magic: 0x%08X\n"
         "  version: %u.%u\n"
         "  asset_count: %u\n"
         "  asset_table_offset: %llu\n"
         "  string_table_offset: %llu\n"
         "  string_table_size: %llu\n"
         "  data_section_offset: %llu\n"
         "  data_section_size: %llu\n"
         "  reserved: %llu\n",
         header.magic, header.version_major, header.version_minor,
         header.asset_count, (unsigned long long)header.asset_table_offset,
         (unsigned long long)header.string_table_offset,
         (unsigned long long)header.string_table_size,
         (unsigned long long)header.data_section_offset,
         (unsigned long long)header.data_section_size,
         (unsigned long long)header.reserved);

  result = bun_parse_assets(&ctx, &header);

  // TODO: on BUN_OK, print human-readable summary to stdout.
  //     on BUN_MALFORMED / BUN_UNSUPPORTED, print violation list to stderr.
  //     See project brief for output requirements.

  bun_close(&ctx);
  return result;
}

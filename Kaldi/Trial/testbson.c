#include <libbson-1.0/bson.h>
#include <cstdio>
#include <iostream>

int main()
{
	bson_t *b;
	char *j;
	b = BCON_NEW("hello", BCON_UTF8("bson!"));
	j = bson_as_canonical_extended_json(b, NULL);
	printf("%s\n", j);

	bson_free(j);
	bson_destroy(b);

	b = BCON_NEW("fuck", BCON_UTF8("you!"), "leather", BCON_UTF8("man"));
	const uint8_t * b_data = bson_get_data(b);

	bson_iter_t iter;

	bson_t *b2;
	if ((b2 = bson_new_from_data(b_data, b->len))) {
		if (bson_iter_init(&iter, b)) {
			while (bson_iter_next(&iter)) {
				printf("Found key: \"%s\"\n", bson_iter_key(&iter));
			}
		}
		bson_destroy(b2);
	}

	bson_iter_t key_fuck;
	if (bson_iter_init(&iter, b) &&
		bson_iter_find_descendant(&iter, "fuck", &key_fuck) &&
		BSON_ITER_HOLDS_UTF8(&key_fuck)) {
		uint32_t len;
		std::cout << "fuck: " << bson_iter_utf8(&key_fuck, &len) << std::endl;
	}	

	

	return 0;
}

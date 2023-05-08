TEST_DIR = Test

.PHONY: all test clean

test:
	make -C $(TEST_DIR)

clean_test:
	make -C $(TEST_DIR) clean

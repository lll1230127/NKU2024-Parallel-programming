
# 可执行文件名
TARGET = neon-align
# 输入参数范围（从10到1000，每次增加10）
N_START = 10
N_END = 1000
N_STEP = 10

.PHONY: all clean test

all: $(TARGET)

test:
	@for n in `seq $(N_START) $(N_STEP) $(N_END)`; do \
		./$(TARGET) $$n; \
	done

clean:
	rm -f $(TARGET)

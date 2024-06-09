
# 可执行文件名
TARGET = mpi
# 输入参数范围（从10到1000，每次增加10）
N_START = 100
N_END = 3000
N_STEP = 100
NP = 8

.PHONY: all clean test

all: $(TARGET)

test: all
	@for n in `seq $(N_START) $(N_STEP) $(N_END)`; do \
		mpirun -np $(NP) ./$(TARGET) $$n 2>/dev/null; \
	done

clean:
	rm -f $(TARGET)

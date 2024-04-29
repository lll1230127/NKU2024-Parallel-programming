# Executable file name
TARGET = sse
# Input parameter range (from 10 to 1000, increasing by 10 each time)
N_START = 10
N_END = 1000
N_STEP = 10

.PHONY: all clean test

all: $(TARGET)

test:
	@for /l %%n in ($(N_START),$(N_STEP),$(N_END)) do ( \
		$(TARGET) %%n \
	)

clean:
	del /q $(TARGET)

# 컴파일러 설정
CXX := g++
CXXFLAGS := $(shell root-config --cflags)
LDFLAGS := $(shell root-config --glibs)

# 실행 파일 이름
TARGET := DAMSAPlotterApp

# 소스 및 객체 파일
SRCS := main.cxx DAMSAPlotter.cxx DAMSAPlotter_Dict.cxx
OBJS := $(SRCS:.cxx=.o)

# 빌드 규칙
all: $(TARGET)

# Dictionary 파일 생성 규칙
DAMSAPlotter_Dict.cxx: DAMSAPlotter.h LinkDef.h
	rootcling -f $@ -c $^

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 클린 빌드
clean:
	rm -f $(TARGET) $(OBJS) DAMSAPlotter_Dict.*


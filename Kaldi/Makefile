IDIR=Include
SRC=Source
TEST=Test
CC=g++
CFLAGS=-I$(IDIR) -Ilibbson-1.0 -lbson-1.0 -lhiredis -levent -pthread -std=c++17 -lm -ldl

KALDI_ROOT=/usr/local/kaldi
KALDI_INCLUDE=-I$(KALDI_ROOT)/src -I$(KALDI_ROOT)/tools -I$(KALDI_ROOT)/tools/openfst/include -I$(KALDI_ROOT)/tools/ATLAS_headers/include

KALDI_FLAGS=$(KALDI_INCLUDE)\
		-DKALDI_DOUBLEPRECISION=0 \
		-DHAVE_EXECINFO_H=1 \
		-DHAVE_CXXABI_H \
		-DHAVE_ATLAS \
		-msse \
		-msse2 \
		-pthread -g \
		-Wl,-rpath=$(KALDI_ROOT)/tools/openfst/lib \
		-rdynamic
_KALDI_STATICS=/online2/kaldi-online2.a /ivector/kaldi-ivector.a \
	    /nnet3/kaldi-nnet3.a /chain/kaldi-chain.a \
	    /nnet2/kaldi-nnet2.a /cudamatrix/kaldi-cudamatrix.a \
	    /decoder/kaldi-decoder.a /lat/kaldi-lat.a \
	    /fstext/kaldi-fstext.a /hmm/kaldi-hmm.a \
	    /feat/kaldi-feat.a /transform/kaldi-transform.a \
	    /gmm/kaldi-gmm.a /tree/kaldi-tree.a \
	    /util/kaldi-util.a /matrix/kaldi-matrix.a \
	    /base/kaldi-base.a
KALDI_STATICS=$(patsubst %,$(KALDI_ROOT)/src/%,$(_KALDI_STATICS))

KALDI_DYNAMICS=$(KALDI_ROOT)/tools/openfst/lib/libfst.so \
	    	/usr/lib/libatlas.so.3 \
		/usr/lib/libf77blas.so.3 \
		/usr/lib/libcblas.so.3 \
		/usr/lib/liblapack_atlas.so.3

KALDI_API=$(KALDI_ROOT)/src/online2/online2-wav-nnet3-latgen-faster-api.o 
_KALDI_OUTS=online-gmm-decodable.o online-feature-pipeline.o online-ivector-feature.o online-nnet2-feature-pipeline.o online-gmm-decoding.o online-timing.o online-endpoint.o onlinebin-util.o online-speex-wrapper.o online-nnet2-decoding.o online-nnet2-decoding-threaded.o online-nnet3-decoding.o online2-wav-nnet3-latgen-faster-api.o  
KALDI_OUTS=$(patsubst %,$(KALDI_ROOT)/src/online2/%,$(_KALDI_OUTS))

ODIR=Bin

_DEPS=$(shell ls $(IDIR))
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

SRCS:=$(shell ls $(SRC) && ls $(TEST))

_OBJ=$(patsubst %.cpp,$(ODIR)/%.o,$(SRCS))

_MAIN=testDecoder.o tests.o main.o # .o with main must be here
MAIN=$(patsubst %.o,$(ODIR)/%.o, $(_MAIN))

OBJ=$(filter-out $(MAIN),$(_OBJ)) # avoid re-containing main


$(ODIR)/%.o: $(SRC)/%.cpp $(DEPS) $(KALDI_API) $(KALDI_OUTS)
	$(CC) -c -o $@ $< $(CFLAGS) $(KALDI_FLAGS) $(KALDI_STATICS) $(KALDI_DYNAMICS)

$(ODIR)/%.o: $(TEST)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tests.out: $(OBJ) $(ODIR)/tests.o
	$(CC) -o $@ $^ $(CFLAGS)

testDecoder.out: $(OBJ) $(ODIR)/testDecoder.o
	$(CC) -o $@ $^ $(CFLAGS)	

kaldi-service: $(OBJ) $(ODIR)/main.o $(KALDI_API) $(KALDI_OUTS) 
	$(CC) -o $@ $^ $(CFLAGS) $(KALDI_FLAGS) $(KALDI_STATICS) $(KALDI_DYNAMICS) 

debug:
	echo "$(OBJ) $(ODIR)/main.o $(KALDI_API)"  && echo "$(OBJ)" && echo "$(ODIR)/"
debug_obj:
	echo "$(OBJ)" &&  echo "$(MAIN)"

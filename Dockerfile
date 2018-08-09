FROM microsoft/dotnet:aspnetcore-runtime AS build-kaldi

ENV CPU_CORE 4

RUN \
  apt-get update -qq && \
  apt-get install -y \
    git bzip2 wget \
    g++ make python python3 \
    zlib1g-dev automake autoconf libtool subversion \
    libatlas-base-dev patch sox


WORKDIR /usr/local/
# Use the newest kaldi version
RUN git clone https://github.com/kaldi-asr/kaldi.git


WORKDIR /usr/local/kaldi/tools
RUN extras/check_dependencies.sh
RUN make -j $CPU_CORE

WORKDIR /usr/local/kaldi/src
RUN ./configure && make depend -j $CPU_CORE && make -j $CPU_CORE

RUN apt-get install -y tree vim

# Copy cvte to Docker
# COPY cvte /usr/local/kaldi/egs/cvte 
# RUN ln -s /usr/local/kaldi/egs/wsj/s5/steps /usr/local/kaldi/egs/cvte/s5/steps
# RUN ln -s /usr/local/kaldi/egs/wsj/s5/utils /usr/local/kaldi/egs/cvte/s5/utils

# Build dotnet app
FROM microsoft/dotnet:sdk AS build-env

WORKDIR /app

# Copy csproj and restore as distinct layers
COPY Service/*.csproj ./
RUN dotnet restore

# Copy everything else and build
COPY Service/ ./

RUN apt-get update && apt-get install -y tree
RUN dotnet publish -c Release -o out
RUN echo "\n####Build results: ####" && tree out

# Go back to where we build kaldi
FROM build-kaldi

# Build runtime image
WORKDIR /app
COPY --from=build-env /app/out .
# Temporary
WORKDIR /usr/local/kaldi/egs/cvte
#

ENTRYPOINT ["dotnet", "testdotnetfordocker.dll"]
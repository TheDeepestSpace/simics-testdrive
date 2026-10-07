FROM docker.io/ubuntu:22.04
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential make cmake git curl wget ca-certificates \
    sudo zsh vim procps jq xz-utils python3 python3-pip \
    gcc-riscv64-unknown-elf libnewlib-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

RUN useradd --create-home --shell /bin/zsh dev && \
    usermod --append --groups sudo dev && \
    echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# Public Intel Simics 7 (ISPM + package bundle). URLs are the public downloads
# also used by intel/simulator-bindings CI; the EULA applies:
# https://lemcenter.intel.com/productDownload/?Product=256660e5-a404-4390-b436-f64324d94959
ARG ISPM_URL=https://registrationcenter-download.intel.com/akdlm/IRC_NAS/ead79ef5-28b5-48c7-8d1f-3cde7760798f/intel-simics-package-manager-1.8.3-linux64.tar.gz
ARG SIMICS_PKGS_URL=https://registrationcenter-download.intel.com/akdlm/IRC_NAS/b9edaa91-4c18-4758-b8e2-b6e709270a4c/simics-7-packages-2025-51-linux64.ispm

ENV SIMICS_HOME=/opt/simics
RUN mkdir -p /opt/simics-ispm "$SIMICS_HOME" && chown dev:dev /opt/simics-ispm "$SIMICS_HOME"
USER dev
RUN set -e; cd /tmp && \
    curl -fL -o ispm.tar.gz "$ISPM_URL" && \
    tar -C /opt/simics-ispm --strip-components=1 -xf ispm.tar.gz && \
    curl -fL -o simics.ispm "$SIMICS_PKGS_URL" && \
    /opt/simics-ispm/ispm settings install-dir "$SIMICS_HOME" && \
    /opt/simics-ispm/ispm packages --install-bundle simics.ispm \
        --non-interactive --trust-insecure-packages && \
    rm -f ispm.tar.gz simics.ispm

ENV PATH=/opt/simics-ispm:$PATH
WORKDIR /workspaces/simics-testdrive

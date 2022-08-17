import setuptools

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

setuptools.setup(
    name="fpgaconvnet-hls",
    version="0.1.1",
    author="Alex Montgomerie",
    author_email="am9215@ic.ac.uk",
    description="Optimiser for mapping convolutional neural network models to FPGA platforms.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/AlexMontgomerie/fpgaconvnet-hls",
    include_package_data=True,
    packages=setuptools.find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3.7',
    install_requires=[
        "fpbinary",
        "untangle",
        "numpy"
    ]
)

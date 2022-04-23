# dpmx_extract_image


Extracts JPEG, BPM files from dpm file

Extracting files from older version of dpm file is known how to make mutator and calculate.

Calculating with and finding mutator from recent version of dpm file is not known and make it difficult to extract files from dpm.

By using JPEG JFIF, JPEG EXIF header, EOI, BMP header and file size, expecting result of caculation with certain mutator can be done.

I just found bitwise calculation by randomly guessing, transforming formula from previous known formula.  
  \
  \
  \
from\
key += mutator1 ^ (\*buff - mutator2);
  \
  \
to\
key+= (mutator1 ^\*buff)- mutator2;  
  \
  \
  \
However, could not find way to make mutator so it take max 2^16 times to find a proper mutator.

Only JPEG JFIF, JPEG EXIF, BMP file is written.

reference: asmodean's reverse engineering page, exdpmx.cpp

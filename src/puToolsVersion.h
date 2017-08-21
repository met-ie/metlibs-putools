#ifndef METLIBS_PUTOOLS_VERSION_H
#define METLIBS_PUTOOLS_VERSION_H

#define METLIBS_PUTOOLS_VERSION_MAJOR 7
#define METLIBS_PUTOOLS_VERSION_MINOR 1
#define METLIBS_PUTOOLS_VERSION_PATCH 0

#define METLIBS_PUTOOLS_VERSION_INT(major,minor,patch) \
    (1000000*major + 1000*minor + patch)
#define METLIBS_PUTOOLS_VERSION_CURRENT_INT \
    METLIBS_PUTOOLS_VERSION_INT(METLIBS_PUTOOLS_VERSION_MAJOR, METLIBS_PUTOOLS_VERSION_MINOR, METLIBS_PUTOOLS_VERSION_PATCH)

#endif // METLIBS_PUTOOLS_VERSION_H

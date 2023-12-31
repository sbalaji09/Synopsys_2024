/**********************************************************************
 *
 *  geo_set.c  -- Public routines for GEOTIFF GeoKey access.
 *
 *    Written By: Niles D. Ritter.
 *
 *  copyright (c) 1995   Niles D. Ritter
 *
 *  Permission granted to use this software, so long as this copyright
 *  notice accompanies any products derived therefrom.
 *
 **********************************************************************/

#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "geotiff.h"   /* public interface        */
#include "geo_tiffp.h" /* external TIFF interface */
#include "geo_keyp.h"  /* private interface       */

/**
This function writes a geokey_t value to a GeoTIFF file.

@param gtif The geotiff information handle from GTIFNew().

@param keyID The geokey_t name (such as ProjectedCSTypeGeoKey).
This must come from the list of legal geokey_t values
(an enumeration) listed below.

@param type Type of the key.

@param count Indicates how many values
to read.  At this time all keys except for strings have only one value,
so <b>index</b> should be zero, and <b>count</b> should be one.<p>

The <b>keyID</b> indicates the key name to be written to the
file and should from the geokey_t enumeration
(eg. <tt>ProjectedCSTypeGeoKey</tt>).  The full list of possible geokey_t
values can be found in geokeys.inc, or in the online documentation for
GTIFKeyGet().<p>

The <b>type</b> should be one of TYPE_SHORT, TYPE_ASCII, or TYPE_DOUBLE and
will indicate the type of value being passed at the end of the argument
list (the key value).  The <b>count</b> should be one except for strings
when it should be the length of the string (or zero to for this to be
computed internally).  As a special case a <b>count</b> of -1 can be
used to request an existing key be deleted, in which no value is passed.<p>

The actual value is passed at the end of the argument list, and should be
a short, a double, or a char * value.  Note that short and double values
are passed by value rather than as pointers when count is 1, but as pointers
if count is larger than 1.<p>

Note that key values aren't actually flushed to the file until
GTIFWriteKeys() is called.  Till then
the new values are just kept with the GTIF structure.<p>

<b>Example:</b><p>

<pre>
    GTIFKeySet(gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1,
               RasterPixelIsArea);
    GTIFKeySet(gtif, GTCitationGeoKey, TYPE_ASCII, 0,
               "UTM 11 North / NAD27" );
</pre>

 */

int GTIFKeySet(GTIF *gtif, geokey_t keyID, tagtype_t type, int count,...)
{
    va_list ap;
    int nIndex = gtif->gt_keyindex[ keyID ];
    GeoKey *key;
    char *data = NULL;
    char *val = NULL;
    pinfo_t sval;
    double dval;

    va_start(ap, count);
    /* pass singleton keys by value */
    if (count>1 && type!=TYPE_ASCII)
    {
        val = va_arg(ap, char*);
    }
    else if( count == -1 )
    {
        /* delete the indicated tag */
        va_end(ap);

        if( nIndex < 1 )
            return 0;

        if (gtif->gt_keys[nIndex].gk_type == TYPE_ASCII)
        {
            _GTIFFree (gtif->gt_keys[nIndex].gk_data);
        }

        while( nIndex < gtif->gt_num_keys )
        {
            _GTIFmemcpy( gtif->gt_keys + nIndex,
                         gtif->gt_keys + nIndex + 1,
                         sizeof(GeoKey) );
            gtif->gt_keyindex[gtif->gt_keys[nIndex].gk_key] = nIndex;
            nIndex++;
        }

        gtif->gt_num_keys--;
        gtif->gt_nshorts -= sizeof(KeyEntry)/sizeof(pinfo_t);
        gtif->gt_keyindex[keyID] = 0;
        gtif->gt_flags |= FLAG_FILE_MODIFIED;

        return 1;
    }
    else switch (type)
    {
      case TYPE_SHORT:
        /* cppcheck-suppress unreadVariable */
        sval=(pinfo_t) va_arg(ap, int);
        val=(char *)&sval;
        break;
      case TYPE_DOUBLE:
        /* cppcheck-suppress unreadVariable */
        dval=va_arg(ap, dblparam_t);
        val=(char *)&dval;
        break;
      case TYPE_ASCII:
        val=va_arg(ap, char*);
        count = (int)strlen(val) + 1; /* force = string length */
        break;
      default:
        assert( FALSE );
        break;
    }
    va_end(ap);

    /* We assume here that there are no multi-valued SHORTS ! */
    if (nIndex)
    {
        /* Key already exists */
        key = gtif->gt_keys+nIndex;
        if (type!=key->gk_type || count > key->gk_count)
        {
            /* need to reset data pointer */
            key->gk_type = type;
            key->gk_count = count;
            key->gk_size = _gtiff_size[ type ];

            if( type == TYPE_DOUBLE )
            {
                key->gk_data = (char *)(gtif->gt_double + gtif->gt_ndoubles);
                gtif->gt_ndoubles += count;
            }
        }
    }
    else
    {
        /* We need to create the key */
        if (gtif->gt_num_keys == MAX_KEYS) return 0;
        key = gtif->gt_keys + ++gtif->gt_num_keys;
        nIndex = gtif->gt_num_keys;
        gtif->gt_keyindex[ keyID ] = nIndex;
        key->gk_key = keyID;
        key->gk_type = type;
        key->gk_count = count;
        key->gk_size = _gtiff_size[ type ];
        if ((geokey_t)gtif->gt_keymin > keyID)  gtif->gt_keymin=keyID;
        if ((geokey_t)gtif->gt_keymax < keyID)  gtif->gt_keymax=keyID;
        gtif->gt_nshorts += sizeof(KeyEntry)/sizeof(pinfo_t);
        if( type == TYPE_DOUBLE )
        {
            key->gk_data = (char *)(gtif->gt_double + gtif->gt_ndoubles);
            gtif->gt_ndoubles += count;
        }
    }

    switch (type)
    {
        case TYPE_SHORT:
            if (count > 1) return 0;
            data = (char *)&key->gk_data; /* store value *in* data */
            break;
        case TYPE_DOUBLE:
            data = key->gk_data;
            break;
        case TYPE_ASCII:
            /* throw away existing data and allocate room for new data */
            if (key->gk_data != 0)
            {
                _GTIFFree(key->gk_data);
            }
            key->gk_data = (char *)_GTIFcalloc(count);
            key->gk_count = count;
            data = key->gk_data;
            break;
        default:
            return 0;
    }

    _GTIFmemcpy(data, val, count*key->gk_size);

    gtif->gt_flags |= FLAG_FILE_MODIFIED;
    return 1;
}

/* Set the version numbers of the GeoTIFF directory */
int  GTIFSetVersionNumbers(GTIF* gtif,
                           unsigned short version,
                           unsigned short key_revision,
                           unsigned short minor_revision)
{
    gtif->gt_version = version;
    gtif->gt_rev_major = key_revision;
    gtif->gt_rev_minor = minor_revision;
    return 1;
}

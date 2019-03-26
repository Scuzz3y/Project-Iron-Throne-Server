outDir="./schema-include"
includeDir="./schema/"
flatbufferSchema="./schema/anomaly_api.fbs"

./flatc --cpp --gen-all --gen-mutable --gen-object-api --gen-compare -I $includeDir -o $outDir $flatbufferSchema

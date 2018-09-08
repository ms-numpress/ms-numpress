ApiKey=$1
nuget pack MSNumpress.csproj -Verbosity detailed
nuget push *.nupkg -ApiKey $NUGET_API_KEY -Source https://api.nuget.org/v3/index.json -Verbosity detailed

ApiKey=$1

CURRENT_TAG=$(git describe --abbrev=0 --tags)
COMMITS_SINCE_TAG=$(git rev-list $CURRENT_TAG.. --count)
VERSION_NUMBER=$(echo $CURRENT_TAG.$COMMITS_SINCE_TAG | sed 's/MsNumpress-//')

nuget pack MSNumpress.csproj -Verbosity detailed -Prop Configuration=Release -Version $VERSION_NUMBER -MSBuildPath /usr/lib/mono/msbuild/15.0/bin/
nuget push *.nupkg -ApiKey $NUGET_API_KEY -Source https://api.nuget.org/v3/index.json -Verbosity detailed

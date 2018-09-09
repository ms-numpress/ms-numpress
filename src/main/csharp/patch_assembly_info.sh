CURRENT_TAG=$(git describe --abbrev=0 --tags)
COMMITS_SINCE_TAG=$(git rev-list $CURRENT_TAG.. --count)
VERSION_NUMBER=$(echo $CURRENT_TAG.$COMMITS_SINCE_TAG | sed 's/MsNumpress-//')
sed -i -e "s/0.0.0.0/$VERSION_NUMBER/g" Properties/AssemblyInfo.cs

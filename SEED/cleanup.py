import os
import xml.etree.ElementTree as ET

# .vcxproj ファイルを指定（プロジェクトのディレクトリに置いて実行）
vcxproj_path = "SEED.vcxproj"

# XML パース
tree = ET.parse(vcxproj_path)
root = tree.getroot()

# VC++ の XML 名前空間
namespace = {"ns": "http://schemas.microsoft.com/developer/msbuild/2003"}

# 不要なファイルリスト
removed_files = []

# ClCompile, ClInclude などの項目をチェック
for item_group in root.findall("ns:ItemGroup", namespace):
    for tag in ["ns:ClCompile", "ns:ClInclude"]:
        for elem in item_group.findall(tag, namespace):
            file_path = elem.attrib["Include"]
            if not os.path.exists(file_path):  # 実際にファイルがない場合
                item_group.remove(elem)
                removed_files.append(file_path)

# 変更を保存
if removed_files:
    tree.write(vcxproj_path, encoding="utf-8", xml_declaration=True)
    print("削除されたファイル:")
    for f in removed_files:
        print(f)
else:
    print("削除するファイルはありません。")

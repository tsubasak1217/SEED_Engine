using System;
using System.IO;
using System.Linq;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;

class ScriptCompiler {


    public static bool CompileScripts() {

        // コンパイルするスクリプトが格納されているディレクトリ一覧
        var scriptDirs = new[]{
            Path.GetFullPath(Path.Combine(AppContext.BaseDirectory, @"..\..\..\..\GameScripts\Interfaces")),
            Path.GetFullPath(Path.Combine(AppContext.BaseDirectory, @"..\..\..\..\..\..\..\..\..\Resources\Scripts"))
        };

        // .csファイルを再帰的に取得
        var sourceFiles = scriptDirs
            .SelectMany(dir => Directory.GetFiles(dir, "*.cs", SearchOption.AllDirectories))
            .ToArray();

        // ソースファイルが見つからない場合の処理
        if(sourceFiles.Length == 0) {
            Console.WriteLine("スクリプトファイルが見つかりませんでした。");
            return false;
        }

        // ソースコードを SyntaxTree に変換
        var syntaxTrees = sourceFiles
            .Select(file => CSharpSyntaxTree.ParseText(File.ReadAllText(file), path: file))
            .ToList();


        // 実行中の .NET の trusted フレームワークアセンブリを解決する
        var trustedAssembliesPath = ((string)AppContext.GetData("TRUSTED_PLATFORM_ASSEMBLIES"))!;
        var references = trustedAssembliesPath.Split(Path.PathSeparator)
            .Where(path =>
                path.EndsWith("System.Runtime.dll") ||
                path.EndsWith("netstandard.dll") ||
                path.EndsWith("System.Console.dll") ||
                path.EndsWith("System.Private.CoreLib.dll") ||
                path.EndsWith("System.Linq.dll") ||
                path.EndsWith("System.Private.Uri.dll") ||
                path.EndsWith("System.Collections.dll"))
            .Select(path => MetadataReference.CreateFromFile(path))
            .ToList();

        // コンパイル設定
        var compilation = CSharpCompilation.Create(
            "ScriptAssembly",
            syntaxTrees,
            references,
            new CSharpCompilationOptions(OutputKind.DynamicallyLinkedLibrary)
        );

        // 出力先 DLL のパス
        var outputPath = Path.Combine(AppContext.BaseDirectory, AppContext.BaseDirectory, @"..\..\..\..\..\Output\CompiledScripts.dll");

        // 実際のビルド
        var result = compilation.Emit(outputPath);

        if(result.Success) {
            Console.WriteLine("コンパイル成功: " + outputPath);
            return true;
        } else {
            Console.WriteLine("コンパイル失敗:");
            foreach(var diagnostic in result.Diagnostics)
                Console.WriteLine(diagnostic.ToString());
        }

        return false;
    }
}
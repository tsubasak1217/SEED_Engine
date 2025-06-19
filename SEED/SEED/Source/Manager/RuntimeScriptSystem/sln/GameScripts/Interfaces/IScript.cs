using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

// スクリプトのインターフェイス
public abstract class IScript {
    // インスタンス作成時の処理
    public abstract void OnStart();
    // フレーム開始時の処理
    public virtual void BeginFrame() { }
    // 通常更新処理
    public abstract void Update();
    // 遅らせ更新処理
    public virtual void LateUpdate() { }
    // フレーム終了時の処理
    public virtual void EndFrame() { }
    // インスタンス破棄時の処理
    public virtual void OnDestroy() { }
}
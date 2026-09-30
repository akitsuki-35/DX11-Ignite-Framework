# DirectX11によるゲーム開発フレームワーク

DirectX11用 自作ゲームフレームワーク "IGNITE"<br>
開発環境：Visual Studio 2026<br>
言語：C++20

## 概要
自作フレームワーク「IGNITE」<br>
DirectX11 / C++20によるゲーム開発用フレームワーク。サブモジュールとしてゲーム開発プロジェクトに導入し使用します。

## 機能
* システムタイマー・システムウィンドウ・ゲームループの基盤を提供します。
* Flyweightを用いた高速・安全なリソース管理を提供します。
* ファイル読み込み・ファイルパス変換等のユーティリティを提供します。
* ゲームオブジェクトに紐づき、汎用的なサブシステム・描画を導入するコンポーネント機能を提供します。
* 自作パーティクルシミュレーター[ https://github.com/akitsuki-35/DX11-Particle-Simulator ]と連携し、csvファイルからパーティクルパラメータを読み込む機能を提供します。

## 開発実績
* 「A.X.I.A」[ https://github.com/akitsuki-35/DX11-GameProject-AXIA ]
   * ジャンル：3Dシューティングゲーム
   * 開発期間：3ヶ月
   * 制作人数：1人

## ブランチ
* master　…　安定版のdevelopをマージ
* develop　…　futureの内容をマージ
* future/　…　作業用ブランチ

## 命名規則

### 汎用
| Type | Style | Sample |
| ---- | ---- | ---- |
| ローカル変数 | camelCase | valueName |
| グローバル変数 | g + PascalCase | gValueName |
| 関数 | PascalCase | FunctionName |
| 引数 | camelCase | valueName |
| ポインタ変数 | _ + camelCase or PascalCase | _valueName / _mValueName |
| 定数 | UPPER_SNAKE_CASE | VALUE_NAME |
| 名前空間 | PascalCase | NameSpace |

### クラス
| Type | Style | Sample |
| ---- | ---- | ---- |
| クラス | PascalCase | ClassName |
| メンバ変数 | m + PascalCase | mClassValue |
| publicメンバ関数 | PascalCase | FunctionName |
| privateメンバ関数 | camelCase | functionName |

### 列挙体
| Type | Style | Sample |
| ---- | ---- | ---- |
| 列挙体 | PascalCase | EnumName |
| 列挙体変数 | PascalCase | EnumValue |

### 構造体
| Type | Style | Sample |
| ---- | ---- | ---- |
| 構造体 | UPPER_SNAKE_CASE | STRUCT_NAME |
| 構造体変数 | PascalCase | StructValue |

## ディレクトリ
### フォルダ
* src
   * ソースファイル
* external
   * 外部インクルードファイル・ライブラリ
* lib
   * 外部ライブラリ
* assets
   * オーディオ・テクスチャ・シェーダーファイル
* _bin
   * 実行ファイル
* _obj
   * ビルド一時ファイル

### 重要ファイル
* .gitignore
   * コミット除外ファイルの指定

# コミットメッセージルール
* 変更内容を示すprefix（接頭辞）を必ず記述する
   * 1つのコミットメッセージに複数のprefixを記述してもよい
* コミットメッセージは変更内容を具体的に記述する

### コミットメッセージフォーマット
* [prefix] + （変更内容を記述したコミットメッセージ）
* prefix: + （変更内容を記述したコミットメッセージ）

### prefix

| prefix | 説明 |
| ---- | ---- |
| add | 新機能・ファイルの追加 |
| fix | バグの修正 |
| update | バグ修正ではない既存機能・ファイルの修正 |
| style | コードのフォーマット修正（改行・インデント等）|
| refactor | リファクタリング（外部から見た機能変更を行わないコード修正）|
| rename | ファイル・フォルダ名の変更 |
| delete | ファイル・機能の削除 |
| move | ファイルの移動 |
| revert | 以前のコミット内容へ差戻し |
| perf | コードの最適化 |
| test | テストの追加・修正 |
| docs | README, 他テキストファイルの修正 |
| merge | master, developへのマージ |
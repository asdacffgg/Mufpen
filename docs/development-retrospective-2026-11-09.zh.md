# Mufpen 开发踩坑与流程复盘（2026-11-07 ~ 11-09）

这份文档记录从 Muffin 改名为 Mufpen 之后这三天的真实问题：构建、调试、测试、打包、
重构各踩过什么坑，哪些是环境问题、哪些是代码问题，以及最后沉淀下来的固定开发流程。

目的不是记流水账，而是把“下次还会遇到”的部分变成规则，把“只发生一次”的部分留在案例里。

---

## 1. 这段时间做了什么

| 工作 | 结果 |
|:--|:--|
| 全仓库 `Muffin → Mufpen` 改名 | 1,384 个文件修改，31 个文件重命名；15 个翻译目录 |
| 本地目录改名 + 构建树修复 | `~/.../Muffin → ~/.../Mufpen`，构建树 10,251 个绝对路径重写 |
| CodeGraph 重新索引 | 1,789 文件 / 43,360 符号 |
| 更新签名密钥与配置迁移 | 复用同一把本地密钥；修正 CNG blob magic |
| Linux ASan CI 修复 | 提交 `24aa85e3` |
| 对齐 MarkText 的打开策略 | 提交 `53d0bdb4` |
| 图片标签 + 编辑器内 hover 预览 | 提交 `cb752849` |
| 构建加速与 AGENTS.md | 提交 `45273b15` |
| 功能展示文档 | 提交 `d09e5baf` |
| 全量测试 | 370 / 370 通过 |

---

## 2. 构建与工具链

### 2.1 `-j16` 被 OOM kill

**现象**：全量构建跑到一半直接退出，`Exit code 137`（SIGKILL）。

**根因**：机器 16 核 / 15 GB。Conan 生成的 preset 默认 `jobs: 16`，Qt/C++ 的翻译单元
峰值内存很高，16 个并发编译单元会把内存吃穿，被 OOM killer 杀掉。

**处理**：

```bash
cmake --build --preset conan-release --target Mufpen -j 8
```

`-j8` 在这台机器上明显快于 `-j4`，又不会像 `-j16` 那样稳定触发 OOM。

**规则**：本机默认 `-j8`；不要用未限流的 preset；不要用 `-j16`。

### 2.2 为什么感觉“每次都在全量编译”

不是每次都全量，而是这轮改动同时踩了四个放大器：

1. 改的是被大量文件包含的头文件：`EditorView.h`、`DocumentTab.h`、
   `CursorPosition.h`、`InlineLayout.h`、新增的 `ImageView.h`；
2. `CMakeLists.txt` 增加了新源文件，触发重新配置；
3. `cmake --build --preset conan-release` 不带 `--target` 时会构建**所有**目标，
   包括 300 多个测试；
4. 当时没有 ccache，生成器还是 Unix Makefiles。

**处理**：

```bash
cmake --preset conan-release \
  -DCMAKE_C_COMPILER_LAUNCHER=ccache \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

cmake --build --preset conan-release --target Mufpen -j 8
cmake --build --preset conan-release --target MufpenDocumentTabsTest -j 8
ctest --preset conan-release -j 8 --output-on-failure
```

注意 ccache 的第一次构建仍然是全量，之后才命中缓存。切换到 Ninja 需要新的 build
目录，同样会全量一次，所以没有特殊理由不要中途换生成器。

**规则**：默认按目标增量构建；只有确实需要全量回归时才构建全部测试。

### 2.3 `ctest | tail` 吃掉了真实退出码

**现象**：一次全量测试报告“exit 0”，实际有 5 个测试失败。

**根因**：`ctest ... 2>&1 | tail -100` 的退出码是 `tail` 的，不是 `ctest` 的。
在没有 `set -o pipefail` 的 shell 里，管道状态天然会掩盖上游失败。

**处理**：

```bash
ctest --preset conan-release -j 8 --output-on-failure 2>&1 | tail -20
rc=${PIPESTATUS[0]}
```

或者先重定向到文件，再检查 `$?`：

```bash
ctest --preset conan-release --output-on-failure > /tmp/ctest.log 2>&1
rc=$?
```

**规则**：任何带管道的构建/测试命令，都必须显式检查 `${PIPESTATUS[0]}` 或先落盘。

### 2.4 目录改名会让构建树失效

**现象**：把 checkout 从 `Muffin` 改名为 `Mufpen` 后，增量构建找不到源文件；
测试里的编译定义仍然指向旧路径。

**根因**：CMake 的构建树里嵌入了绝对路径（这次是 10,251 处），compile definitions
里也有绝对源码路径。

**处理**：

1. 重新运行 `cmake --preset conan-release`；
2. 重新全量构建（包括测试），因为编译定义变了；
3. `codegraph index .` 重建索引。

**规则**：移动/重命名 checkout 属于迁移，不是增量操作；必须 reconfigure + 全量构建 +
重新索引。

### 2.5 `build/Release`、`build/dist`、已安装应用是三份不同的东西

**现象**：排查内存时用户运行的是 `~/.local/opt/Muffin/Muffin`，这是 11 月 7 日安装的
旧快照，进程名还是 Muffin；而当前工作树的产物在 `build/Release/Mufpen`。结论一度建立在
错误的二进制上。

另外，`cmake --build ...` 只更新 `build/Release`，不会更新 `build/dist`。

**处理**：

```bash
cmake --build --preset conan-release --target dist -j 8
scripts/install-local.sh --skip-build
readlink -f "$(command -v mufpen)"
pgrep -a -x Mufpen
```

**规则**：排查运行问题时，先确认进程的可执行文件路径；要验证“安装后的行为”，必须走
`dist` / 安装脚本，而不是直接跑 `build/Release`。

### 2.6 后台长构建 + 机器重启 = 会话中断

**现象**：改名后的全量构建在后台运行，机器重启，构建被杀；Claude 会话也在另一个项目
目录下（目录已改名），续接时需要从 JSONL transcript 恢复上下文。

**处理**：找到旧项目目录下的会话记录，重建“已完成 / 中断点 / 未完成”三件事，再继续。

**规则**：

- 长构建放后台，但在高风险操作（改目录、重启、大改）前先提交或写清楚进度；
- 会话记录是恢复工作的第一手资料，比记忆可靠；
- 恢复时先确认工作树状态（`git status`、`git log`）和中断点，再动手。

### 2.7 CI 的 ASan job 在配置阶段失败

**现象**：`Linux ASan (blocking)` 失败，日志是 CMake 配置错误，不是编译器/测试错误。

**根因**：

- Linux 上 `MUFPEN_BUNDLE_FCITX5_PLUGIN` 默认 ON；
- 该选项会调用 `mufpen_add_fcitx5_qt_plugin()`，它要求 Extra CMake Modules (ECM)；
- ASan job 的 apt 列表里没有 `extra-cmake-modules`，普通 Linux job 有；
- 这个失败在改名之前就存在，不是改名引起的。

**处理（方案 A）**：ASan job 不构建 dist，也不需要输入法插件，直接关闭：

```yaml
cmake --preset conan-release \
  -DMUFPEN_BUNDLE_FCITX5_PLUGIN=OFF \
  ...
```

**规则**：

- 不构建 dist 的 CI job 应关闭只服务 bundle 的可选依赖；
- 判断“是不是这次改动引入的”，先对比上一个 commit 的同一 job 日志，或旧二进制。

---

## 3. 调试与度量

### 3.1 先量化，再上 profiler

排查内存时第一步不是 heaptrack，而是用 `/proc` 确认哪个场景异常：

```bash
pid=$(pgrep -n -x Mufpen)
grep -E 'VmRSS|VmHWM' /proc/$pid/status
grep -E '^(Rss|Pss|Pss_Anon|Private_Dirty):' /proc/$pid/smaps_rollup
```

得到的场景表：

| 场景 | RSS | 私有内存 |
|:--|--:|--:|
| 空载 | ~148 MB | ~36 MB |
| 11 KB 普通文档 | ~164 MB | ~43 MB |
| 1 MiB 块密集压力文档 | ~181 MB | ~63 MB |
| 5.47 MB 单行 JSON，渲染模式 | **~1016 MB** | **~900 MB** |
| 同一 JSON，源码模式 | ~217 MB | ~103 MB |

没有这张表，后面的 heaptrack 结果就无法判断“正常”还是“异常”。

### 3.2 `offscreen` 下的内存数据是假的

**现象**：用 `QT_QPA_PLATFORM=offscreen` 打开 1 MiB 压力文档，测到 ~1 GB；一度以为发现了
严重问题。

**根因**：offscreen 没有真实视口，布局走了全量路径，和真实窗口的行为不同。

**处理**：改用 Xvfb 真实视口：

```bash
Xvfb :99 -screen 0 1280x900x24 &
DISPLAY=:99 XDG_CONFIG_HOME=... ./Mufpen <file>
```

同一份 1 MiB 文档在 Xvfb 下只有 ~181 MB。

**规则**：offscreen 只用于功能测试；内存/性能测量必须在 Xvfb 或真实桌面下做。

### 3.3 跨进程比较要用 PSS 和进程树

**现象**：系统监视器里 MarkText 只有一个进程，看起来内存不高。

**真相**：Electron 实际有 7 个进程：

```text
marktext 主进程   416 MB
renderer          295 MB
gpu-process       195 MB
network utility    80 MB
zygote ×3          61 + 60 + 14 MB
合计 RSS ≈ 1.2 GB，PSS ≈ 754 MB
```

Mufpen 是单进程，任务管理器里一行就是全部。空载 ~148 MB RSS / ~132 MB PSS。

**规则**：跨应用/跨进程比较内存，按进程树求和 PSS（共享库不重复计算），不要只看
任务管理器的一行。

### 3.4 heaptrack：峰值分配 ≠ 内存泄漏

**现象**：关闭 5.4 MB JSON 标签后，RSS 从 1016 MB 只降到 634 MB，怀疑泄漏。

**heaptrack 结果**：

```text
377 MB  QTextEngine::LayoutData::reallocate
189 MB  QTextEngine::attributes()
        ← QTextLine::layout_helper
        ← mufpen::LiteralTextLayout::draw
        ← BlockLayout::paintCodeFence
```

但泄漏报告只有约 **66 KB**。也就是说：

- 峰值是 Qt 文本引擎为 5.4M 字符分配布局/属性数组；
- 关闭后保留的部分主要是缓存和 glibc 分配器保留的空闲页，不是“忘了 free”；
- `MALLOC_TRIM_THRESHOLD_=0` 只能再释放约 158 MB，仍剩 358 MB。

**规则**：

- 定位峰值：`heaptrack_print -a 0 -T 0 -l 0 -p 1 -n 20 -s 8 trace.zst`；
- 定位泄漏：`heaptrack_print -a 0 -T 0 -p 0 -l 1 trace.zst`；
- 两者是两个问题，不要用“泄漏报告很小”否定“峰值异常”。

遗留：这次没有单独实现“关闭超大块后主动释放文本布局/字体缓存”的通用机制；触发路径
已经通过 Markdown 白名单、JSON front matter 规则和超大块防护消除。如果以后仍出现
超大块，需要再单独处理关闭后的缓存释放。

### 3.5 单行 5.4 MB JSON 的 165 倍放大

**现象**：`docs/benchmarks/document-open-2026-10-16.json` 是 5,468,193 字节、只有 1 行的
JSON。打开后私有内存 ~900 MB，约为文件大小的 165 倍。

**根因链**：

1. 打开文件不看扩展名，所有文本都走 Markdown 解析；
2. `scanJsonFrontMatter` 只看内容：以 `{` 开头、能扫描到匹配的 `}`、能被
   `QJsonDocument` 解析成 object，就判定为 front matter；
3. 整个 5.4 MB 文件变成一个 `FrontMatter` literal 块；
4. `LiteralTextLayout` 把整个块当一个段落交给 Qt；
5. `QTextEngine` 按 5.4M 个字符扩容布局数组 → 377 MB + 189 MB。

**对照实验**：

| 输入 | 是否 front matter | RSS |
|:--|:--:|--:|
| 5 MB 单行纯文本 | 否 | 457 MB |
| 5 MB 单行 JSON 数组 | 否 | 529 MB |
| 5 MB JSON 对象，前面有空行 | 否 | 482 MB |
| 5 MB JSON 对象，文件开头就是 `{` | **是** | **1016 MB** |

**修复**：

- 只默认打开 Markdown 白名单（对齐 MarkText）；
- JSON front matter 要求 `{` 后紧跟换行（`{\n`），压缩 JSON 不再算 front matter；
- 文件树、打开对话框、Quick Open、工作区搜索、最近文件、会话恢复统一切到白名单。

**规则**：编辑器不能假设“块”有上限；任何“整文件/整段交给文本引擎”的路径都要有
大小和超长行保护。

### 3.6 安装后发现不了图标：GTK 图标缓存

**现象**：图标文件都在 `~/.local/share/icons/hicolor/*/apps/mufpen.png`，但 Ubuntu
应用列表里显示的是 `image-missing.png`。

**根因**：

- `icon-theme.cache` 是 11 月 1 日生成的，图标是 11 月 7 日才装进去的；
- 本地 `hicolor` 没有 `index.theme`，GTK 判断缓存有效性时没有 mtime 可比；
- GTK 信任旧缓存，不会扫描新增文件。

**复现**：临时目录先生成含 `aaa.png` 的缓存，再加入 `bbb.png` 且不刷新缓存，
`Gtk.IconTheme.lookup_icon("bbb")` 返回 `.../status/image-missing.png`。

**处理**：

```bash
gtk-update-icon-cache -f -t ~/.local/share/icons/hicolor
```

并修正 `scripts/install-local.sh`：安装全部尺寸，安装后自动刷新缓存。

**规则**：安装/更新图标后必须刷新 GTK 缓存；验证要用 `Gtk.IconTheme` 实际 lookup，
不要只看文件是否存在。

### 3.7 环境和凭据：该由用户执行的步骤不要硬闯

- WebFetch 的域名预检失败时，`skipWebFetchPreflight` 属于安全设置，不能擅自写；
- GitHub secret 是写操作且不可读，`gh secret set` 被自动权限分类器拦截后，正确做法是
  把**可直接执行、从 stdin 读密钥**的命令交给用户，而不是想办法绕过；
- 私钥文件权限保持 600，打印时只输出长度/magic，不输出内容。

**规则**：凭据写入、安全设置、权限变更默认交给用户执行，并提供最小、可复核的命令。

---

## 4. 测试

### 4.1 `minimal` 和 `offscreen` 不是同一件事

**现象**：`MufpenSplitViewTest` 在 `QT_QPA_PLATFORM=minimal` 下加 Mermaid hover 后崩溃。

**根因**：`MermaidFontRegistry` 在 minimal 平台注册字体时会走到 `QRawFont` 的失败路径，
这是既有的平台限制。

**处理**：

- 几何/布局测试继续用 `minimal`；
- Mermaid、图片字体/光栅化相关测试单独注册为 `MufpenImagePreviewTest`，用 `offscreen`。

```cmake
add_test(NAME MufpenImagePreviewTest COMMAND $<TARGET_FILE:MufpenDocumentTabsTest> --image-preview)
set_tests_properties(MufpenImagePreviewTest PROPERTIES
  ENVIRONMENT_MODIFICATION "${MUFPEN_TEST_ENVIRONMENT_MODIFICATIONS};QT_QPA_PLATFORM=set:offscreen"
  RESOURCE_LOCK MufpenQtGui)
```

**规则**：按测试实际依赖选择 QPA；Mermaid 测试不要跑在 minimal 下。

### 4.2 像素断言要能容忍字体栅格化差异

**现象**：`MufpenRenderHeadingFitContentTest` 断言选中文本“变红”的像素数 `> 20`，
实际 `selRed=18`、`selInk=43`，测试失败。

**判断**：用改名前的旧二进制复跑，得到同样的 `selRed=18`，说明是既有的字体/光栅化
环境差异，不是改名或功能回归。

**修复**：

```cpp
require(selRed * 3 >= selInk, ...);
```

**规则**：

- 像素测试断言“关系/比例”，不要绑定固定像素数；
- 判断失败是否既有：用旧二进制或上一个 commit 复跑同样场景。

### 4.3 测试不要依赖写图插件

**现象**：图片测试里用 `QImage::save()` 生成 JPEG，失败——测试 bundle 里没有 JPEG
writer 插件。

**处理**：改用只读 fixture `tests/fixtures/decode_test.jpg`，通过 app-dir 相对路径解析。

**规则**：测试优先使用仓库内只读 fixture，不依赖运行环境的编码器插件。

### 4.4 失败状态必须清空模型

**现象**：`ImageView::loadFile` 加载损坏图片失败后，界面仍显示上一张图片。

**修复**：失败路径同时清空 `image_`、`sourcePath_`、`sourceUrl_`，并重置 pixmap。

**规则**：错误页不只是“显示错误文案”，还要清掉旧状态，避免 UI 和模型不一致。

### 4.5 新源文件加错了目标

**现象**：`ImageView.cpp` 被加进了 `MUFPEN_TRANSLATABLE_SOURCES`，没有加进
`add_executable(Mufpen)`，链接时报 `undefined reference`。

**根因**：测试目标的应用源码是从 `Mufpen` 目标派生的；源文件只加进翻译源列表不会参与链接。

**规则**：新增 `.cpp` 先加到应用目标；翻译源列表是额外属性，不是构建列表。

### 4.6 测试注册与全量回归

新增测试需要同时做三件事：

1. 在测试入口注册（如 `--image-preview`）；
2. 在 `cmake/MufpenTests.cmake` 里 `add_test`；
3. 设置正确的 QPA 环境和 `RESOURCE_LOCK`。

验证顺序：先跑目标测试，再跑全量：

```bash
cmake --build --preset conan-release --target MufpenDocumentTabsTest -j 8
ctest --preset conan-release -R MufpenImagePreviewTest --output-on-failure
ctest --preset conan-release -j 8 --output-on-failure
```

---

## 5. 改名与重构的边界

### 5.1 “名字作为标识” vs “名字作为数据”

全仓库改名时最容易出错的是把两者混为一谈：

**应该改**：`muffin::` 命名空间、二进制/库名、CMake 变量、配置目录、注册表键、
ProgID/MIME、安装器、CI、翻译上下文。

**不能改**：

- 上游署名（`jstzwj/Muffin`、`Muffin Contributors`）；
- 词典里的真实单词（`muffin`、`ragamuffin`）；
- 冻结的测试夹具：`MuffinFixture*` 字体族名、黄金样本文本、浏览器记录的字形指标
  和 SHA-256；
- `docs/benchmarks/**` 的历史记录；
- 旧 CNG 密钥容器名（用于兼容查找）。

最终残留扫描剩 120 处，全部属于有意保留。

**规则**：改名前列出“标识”和“数据”两类清单；对数据区设排除规则，并在扫描时显式排除。

### 5.2 哈希、夹具与小写键

- 两个主题 CSS 变量改名后，夹具里记录的 SHA-256 对不上；确认 diff 只是改名后，才更新
  integrity pin；
- 测试源码里的小写 fixture 键被改成 `mufpenfixturesans`，但冻结的 fixture JSON 仍是旧键，
  产生 producer/consumer 不一致；
- 这类问题不会在编译期暴露，只能靠“改完后跑全量 + 交叉检查”。

**规则**：改名后检查所有哈希/指纹/小写键/序列化常量；对 producer 和 consumer 成对审计。

### 5.3 翻译的 lupdate namespace 坑

`lupdate` 无法正确处理 `.cpp` 里的 `namespace mufpen { }`，会把后续 `tr()` 归到错误
context。这条已经写进 `AGENTS.md`：

- 可翻译 `.cpp` 不允许出现任何 `namespace mufpen { }`；
- 方法用 `mufpen::ClassName::method()` 定义；
- 文件局部 helper 放匿名 namespace。

这次 15 个翻译目录最终验证：每个 36 个 `mufpen::` context，0 个残留 `Muffin` 文本。

### 5.4 存储位置和凭据不能“改名”

- `QSettings` 路径、AppDataLocation、注册表键、便携标记文件、更新缓存键，都是持久化
  标识；改名后旧数据不会自动跟过来；
- GitHub secret 是只写的，没有 rename 操作，只能用同一个值在新名字下重建；
- CNG 私钥 blob 的 magic 必须是 `ECS2`，本地脚本曾误写成 `ECSK`，Windows 导入会拒绝。

**规则**：改名清单里单列“持久化标识”；存储迁移与凭据重建要单独设计，不要混在
字符串替换里。

### 5.5 用户明确“不向后兼容”时，停止迁移工作

这次用户明确说明：项目还没发布，所有兼容问题可以忽略。于是：

- 不写旧配置/旧草稿的自动迁移；
- 不为旧路径加 fallback；
- 只保留当前版本的一致性修复。

**规则**：兼容性是否要做是产品决策，不是技术默认；用户明确后，不要在兼容性上继续花时间。

---

## 6. 产品对齐与设计判断

### 6.1 用 MarkText 的真实实现做参考，而不是凭印象

这次涉及打开策略时，先读了 MarkText 的源码/行为，结论是：

- 打开文件只接受目录或 Markdown 白名单；
- 文件树里非 Markdown 不可打开；
- 没有“打开任意文本 + 源码模式”的兜底；
- source-code 模式是单栏 CodeMirror，不是“左边源码、右边实时预览”。

Mufpen 原来的 `Source`/`Split` 是两回事：`Source` 是单栏源码，`Split` 才是双栏。
把“非 md 默认源码模式”当成对齐，是理解错了。

**规则**：涉及“和某编辑器对齐”的需求，先查对方源码/实测，再定方案。

### 6.2 性能表述要有条件

README 原文的“内存占用极低”“百兆文档瞬间打开”没有限定条件。项目自己的 benchmark
记录里，10 MiB 长代码块约 1.37 GiB 私有内存，10 MiB 格式密集文档约 535 MB 工作集。

**规则**：性能数字必须带场景（空载/普通文档/大文档/单行超大文件）和口径（RSS/PSS/
私有内存），否则不要写成无条件结论。

---

## 7. 固定的开发流程

这是这三天实际稳定下来的循环。

### 7.1 开始前

1. `git status --short`、`git branch --show-current`、`git log --oneline -5`；
2. 用 CodeGraph 定位相关代码（`.codegraph/` 存在时优先于 grep/read）；
3. 明确这次的范围和验收标准；如果用户说“先给结论/先汇报”，先不动手构建；
4. 对行为对齐类需求，先读参考实现（如 MarkText）。

### 7.2 修改中

1. 小步修改，保持工作区可编译；
2. 新增源文件先加到应用目标，再考虑翻译源列表；
3. 改头文件要有“会触发大范围重编译”的预期，必要时先说明；
4. 不在可翻译 `.cpp` 里写 `namespace mufpen { }`。

### 7.3 验证

```bash
# 1. 目标构建
cmake --build --preset conan-release --target <Target> -j 8

# 2. 目标测试
ctest --preset conan-release -R <TestName> --output-on-failure

# 3. 全量回归（需要时）
ctest --preset conan-release -j 8 --output-on-failure
# 管道场景务必： rc=${PIPESTATUS[0]}

# 4. 格式与空白
clang-format -i <files>   # 版本以 CI/项目要求为准
git diff --check
```

GUI 功能还要做：

- 真实桌面或 Xvfb 下启动，确认窗口/标签/交互；
- 打包验证走 `--target dist` + `scripts/install-local.sh`，不要只跑 `build/Release`；
- 安装图标后刷新 GTK 缓存。

### 7.4 提交与推送

1. 用户要求提交时才提交；用户说“先报告，我再决定”时严格照做；
2. 提交按主题拆分：功能、测试修复、构建文档分开；
3. **严禁加 Claude Code 作为 co-author**；
4. 推送后检查 CI；CI 失败先取 job 日志，判断是不是本次引入。

### 7.5 收尾

1. 全量测试通过后再报告“完成”；
2. 清理临时脚本、调试输出、未跟踪文件；
3. 把这次新学到的规则写回 `AGENTS.md` 或文档，而不是只留在会话里。

---

## 8. 可固化清单

### 8.1 已经固化

| 规则 | 位置 |
|:--|:--|
| ccache + `-j8` + 按目标构建 | `AGENTS.md` Build acceleration |
| 不要跑未限流的 preset；`-j16` 会 OOM | `AGENTS.md` Build acceleration |
| dist 与 `build/Release` 是两回事 | `AGENTS.md` Package And Run |
| Linux bundle 的 fcitx5 插件与 ECM 依赖 | `AGENTS.md` Linux input methods |
| 可翻译 `.cpp` 不得包 `namespace mufpen { }` | `AGENTS.md` Translation |
| `CLAUDE.md` 只保留 `@AGENTS.md` | `CLAUDE.md` |
| `.codegraph/` 本地索引不入库 | `.gitignore` |

### 8.2 建议写入 `AGENTS.md`（本次已加）

- 管道命令必须检查 `${PIPESTATUS[0]}`；
- `minimal` 用于几何/布局，`offscreen` 用于 Mermaid/图片；Mermaid 不进 minimal；
- 内存/性能测量用 Xvfb/真实视口，比较进程树 PSS；
- 调试运行中的问题先确认进程可执行文件路径；
- 安装图标后刷新 GTK 图标缓存；
- 判断“既有失败”要用旧二进制或上一个 CI run 复现。

### 8.3 值得脚本化

| 脚本 | 作用 |
|:--|:--|
| `scripts/install-local.sh` | 已加入：安装全部图标尺寸 + 刷新 GTK 缓存 |
| 建议：`scripts/dev-verify.sh` | 目标构建 → 目标测试 → 全量 ctest（检查真实退出码）→ `git diff --check` |
| 建议：`scripts/mem-check.sh` | Xvfb 启动、打开指定文档、输出 RSS/PSS/VmHWM，避免重复手写 `/proc` 命令 |

脚本化只做“每次都要跑且命令固定”的部分；一次性迁移和分析仍按文档执行。

### 8.4 只适合留在复盘里的

- 改名时的具体文件数、路径数、翻译目录数；
- CNG `ECSK → ECS2` 这类一次性 bug；
- 单次内存数字（148 MB / 1016 MB 等）——记录**方法**，不要记录成产品承诺；
- GitHub secret 的具体操作过程（保留“由用户执行”的规则即可）。

---

## 附录 A：命令速查

```bash
# 构建
cmake --preset conan-release \
  -DCMAKE_C_COMPILER_LAUNCHER=ccache \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
cmake --build --preset conan-release --target Mufpen -j 8
cmake --build --preset conan-release --target MufpenDocumentTabsTest -j 8

# 测试
ctest --preset conan-release -R MufpenImagePreviewTest --output-on-failure
ctest --preset conan-release -j 8 --output-on-failure
rc=${PIPESTATUS[0]}

# 内存
pid=$(pgrep -n -x Mufpen)
grep -E 'VmRSS|VmHWM' /proc/$pid/status
grep -E '^(Rss|Pss|Pss_Anon|Private_Dirty):' /proc/$pid/smaps_rollup

# heaptrack
heaptrack -o /tmp/hp.zst ./build/Release/Mufpen <file>
heaptrack_print -a 0 -T 0 -l 0 -p 1 -n 20 -s 8 /tmp/hp.zst   # 峰值
heaptrack_print -a 0 -T 0 -p 0 -l 1 /tmp/hp.zst              # 泄漏

# 真实视口
Xvfb :99 -screen 0 1280x900x24 &
DISPLAY=:99 ./build/Release/Mufpen <file>

# 安装与图标
cmake --build --preset conan-release --target dist -j 8
scripts/install-local.sh --skip-build
gtk-update-icon-cache -f -t ~/.local/share/icons/hicolor

# CodeGraph
codegraph index .
codegraph status .
```

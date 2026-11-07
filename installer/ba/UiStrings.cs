using System.Collections.Generic;
using System.Globalization;

namespace Muffin.Setup
{
    /// <summary>
    /// Installer strings, compiled into the BA assembly (no satellite
    /// resources, so the UX container needs no per-culture payloads) and
    /// covering the same languages as the app's own translations.
    ///
    /// The language is picked once at startup from the OS UI culture and can
    /// be changed at runtime from the title-bar selector. Adding a language
    /// = adding one table and one entry in LanguageCodes/LanguageNames.
    /// </summary>
    internal static class UiStrings
    {
        public static readonly string[] LanguageCodes =
        {
            "en", "zh-CN", "zh-TW", "ja", "ko", "de", "fr", "es", "it", "nl", "pl", "pt-BR", "ru", "tr", "vi",
        };

        /// <summary>Native names for the title-bar selector, aligned with LanguageCodes.</summary>
        public static readonly string[] LanguageNames =
        {
            "English", "简体中文", "繁體中文", "日本語", "한국어", "Deutsch", "Français",
            "Español", "Italiano", "Nederlands", "Polski", "Português (BR)", "Русский",
            "Türkçe", "Tiếng Việt",
        };

        private static readonly Dictionary<string, string> English = new Dictionary<string, string>
        {
            ["LanguageAuto"] = "Auto",
            ["WindowTitle"] = "Mufpen Setup",
            ["Detecting"] = "Checking installed versions…",
            ["WelcomeTagline"] = "A quiet place for Markdown. Fast, native, and out of your way.",
            ["Version"] = "Version {0}",
            ["Options"] = "OPTIONS",
            ["AddContextMenu"] = "Add \"Open with Mufpen\" to the Explorer context menu",
            ["AssociateFiles"] = "Show Mufpen in \"Open with\" for Markdown files",
            ["Install"] = "Install",
            ["Update"] = "Update",
            ["Repair"] = "Repair",
            ["Uninstall"] = "Uninstall",
            ["InstalledTitle"] = "Mufpen is installed",
            ["InstalledSub"] = "Version {0} is ready on this computer.",
            ["UninstallTitle"] = "Uninstall Mufpen?",
            ["UninstallBody"] = "Mufpen will be removed from this computer. Your notes, documents, and personal settings are kept.",
            ["Keep"] = "Keep",
            ["Installing"] = "Installing Mufpen…",
            ["Updating"] = "Updating Mufpen…",
            ["Repairing"] = "Repairing Mufpen…",
            ["Removing"] = "Removing Mufpen…",
            ["Cancel"] = "Cancel",
            ["SuccessInstalled"] = "Mufpen is ready",
            ["SuccessRemoved"] = "Mufpen was removed",
            ["RestartNote"] = "A restart is recommended to finish updating system files.",
            ["Launch"] = "Launch Mufpen",
            ["Close"] = "Close",
            ["FailedTitle"] = "Setup could not finish",
            ["OpenLog"] = "Open setup log",
            ["NewerInstalled"] = "A newer version of Mufpen is already installed. Uninstall it first if you want to install this version.",
            ["ErrorFmt"] = "Setup could not finish (error 0x{0:X8}). The log has the details.",
            ["StageCache"] = "Preparing installation files",
            ["StageRegister"] = "Finishing up",
            ["StageUnregister"] = "Removing registration",
            ["StageRemovePrevious"] = "Removing the previous version",
            ["StageRollback"] = "Rolling back changes",
            ["PlanErrorFmt"] = "Setup could not be prepared (error 0x{0:X8}).",
        };

        private static readonly Dictionary<string, string> ChineseSimplified = new Dictionary<string, string>
        {
            ["LanguageAuto"] = "自动",
            ["WindowTitle"] = "Mufpen 安装程序",
            ["Detecting"] = "正在检查已安装的版本…",
            ["WelcomeTagline"] = "安静的 Markdown 写作空间。快速、原生、不打扰。",
            ["Version"] = "版本 {0}",
            ["Options"] = "选项",
            ["AddContextMenu"] = "在资源管理器右键菜单中添加“用 Mufpen 打开”",
            ["AssociateFiles"] = "在 Markdown 文件的“打开方式”中显示 Mufpen",
            ["Install"] = "安装",
            ["Update"] = "更新",
            ["Repair"] = "修复",
            ["Uninstall"] = "卸载",
            ["InstalledTitle"] = "Mufpen 已安装",
            ["InstalledSub"] = "版本 {0} 已就绪。",
            ["UninstallTitle"] = "要卸载 Mufpen 吗？",
            ["UninstallBody"] = "将从这台电脑移除 Mufpen。你的笔记、文档和个人设置会保留。",
            ["Keep"] = "保留",
            ["Installing"] = "正在安装 Mufpen…",
            ["Updating"] = "正在更新 Mufpen…",
            ["Repairing"] = "正在修复 Mufpen…",
            ["Removing"] = "正在移除 Mufpen…",
            ["Cancel"] = "取消",
            ["SuccessInstalled"] = "Mufpen 已就绪",
            ["SuccessRemoved"] = "Mufpen 已移除",
            ["RestartNote"] = "建议重启电脑以完成系统文件更新。",
            ["Launch"] = "启动 Mufpen",
            ["Close"] = "关闭",
            ["FailedTitle"] = "安装未能完成",
            ["OpenLog"] = "打开安装日志",
            ["NewerInstalled"] = "已安装更高版本的 Mufpen。如需安装此版本，请先卸载新版。",
            ["ErrorFmt"] = "安装未能完成（错误 0x{0:X8}）。详细信息请查看日志。",
            ["StageCache"] = "正在准备安装文件",
            ["StageRegister"] = "正在完成收尾",
            ["StageUnregister"] = "正在移除注册信息",
            ["StageRemovePrevious"] = "正在移除旧版本",
            ["StageRollback"] = "正在回滚更改",
            ["PlanErrorFmt"] = "无法准备安装（错误 0x{0:X8}）。",
        };

        private static readonly Dictionary<string, string> ChineseTraditional = new Dictionary<string, string>
        {
            ["LanguageAuto"] = "自動",
            ["WindowTitle"] = "Mufpen 安裝程式",
            ["Detecting"] = "正在檢查已安裝的版本…",
            ["WelcomeTagline"] = "安靜的 Markdown 寫作空間。快速、原生、不打擾。",
            ["Version"] = "版本 {0}",
            ["Options"] = "選項",
            ["AddContextMenu"] = "在檔案總管右鍵選單中加入「以 Mufpen 開啟」",
            ["AssociateFiles"] = "在 Markdown 檔案的「開啟檔案」中顯示 Mufpen",
            ["Install"] = "安裝",
            ["Update"] = "更新",
            ["Repair"] = "修復",
            ["Uninstall"] = "解除安裝",
            ["InstalledTitle"] = "Mufpen 已安裝",
            ["InstalledSub"] = "版本 {0} 已就緒。",
            ["UninstallTitle"] = "要解除安裝 Mufpen 嗎？",
            ["UninstallBody"] = "將從這台電腦移除 Mufpen。你的筆記、文件和個人設定會保留。",
            ["Keep"] = "保留",
            ["Installing"] = "正在安裝 Mufpen…",
            ["Updating"] = "正在更新 Mufpen…",
            ["Repairing"] = "正在修復 Mufpen…",
            ["Removing"] = "正在移除 Mufpen…",
            ["Cancel"] = "取消",
            ["SuccessInstalled"] = "Mufpen 已就緒",
            ["SuccessRemoved"] = "Mufpen 已移除",
            ["RestartNote"] = "建議重新開機以完成系統檔案更新。",
            ["Launch"] = "啟動 Mufpen",
            ["Close"] = "關閉",
            ["FailedTitle"] = "安裝無法完成",
            ["OpenLog"] = "開啟安裝記錄",
            ["NewerInstalled"] = "已安裝較新版本的 Mufpen。如要安裝此版本，請先解除安裝新版。",
            ["ErrorFmt"] = "安裝無法完成（錯誤 0x{0:X8}）。詳細資訊請查看記錄。",
            ["StageCache"] = "正在準備安裝檔案",
            ["StageRegister"] = "正在完成收尾",
            ["StageUnregister"] = "正在移除註冊資訊",
            ["StageRemovePrevious"] = "正在移除舊版本",
            ["StageRollback"] = "正在回復變更",
            ["PlanErrorFmt"] = "無法準備安裝（錯誤 0x{0:X8}）。",
        };

        private static readonly Dictionary<string, string> Japanese = new Dictionary<string, string>
        {
            ["LanguageAuto"] = "自動",
            ["WindowTitle"] = "Mufpen セットアップ",
            ["Detecting"] = "インストール済みバージョンを確認中…",
            ["WelcomeTagline"] = "静かな Markdown 執筆環境。高速・ネイティブ・邪魔をしない。",
            ["Version"] = "バージョン {0}",
            ["Options"] = "オプション",
            ["AddContextMenu"] = "エクスプローラーの右クリックメニューに「Mufpen で開く」を追加",
            ["AssociateFiles"] = "Markdown ファイルの「プログラムから開く」に Mufpen を表示",
            ["Install"] = "インストール",
            ["Update"] = "更新",
            ["Repair"] = "修復",
            ["Uninstall"] = "アンインストール",
            ["InstalledTitle"] = "Mufpen はインストール済みです",
            ["InstalledSub"] = "バージョン {0} がこのコンピューターで利用できます。",
            ["UninstallTitle"] = "Mufpen をアンインストールしますか？",
            ["UninstallBody"] = "このコンピューターから Mufpen を削除します。メモ・ドキュメント・個人設定は保持されます。",
            ["Keep"] = "維持",
            ["Installing"] = "Mufpen をインストールしています…",
            ["Updating"] = "Mufpen を更新しています…",
            ["Repairing"] = "Mufpen を修復しています…",
            ["Removing"] = "Mufpen を削除しています…",
            ["Cancel"] = "キャンセル",
            ["SuccessInstalled"] = "Mufpen の準備ができました",
            ["SuccessRemoved"] = "Mufpen を削除しました",
            ["RestartNote"] = "システムファイルの更新を完了するには再起動をおすすめします。",
            ["Launch"] = "Mufpen を起動",
            ["Close"] = "閉じる",
            ["FailedTitle"] = "セットアップを完了できませんでした",
            ["OpenLog"] = "セットアップログを開く",
            ["NewerInstalled"] = "より新しいバージョンの Mufpen が既にインストールされています。このバージョンをインストールするには先にアンインストールしてください。",
            ["ErrorFmt"] = "セットアップを完了できませんでした（エラー 0x{0:X8}）。詳細はログをご覧ください。",
            ["StageCache"] = "インストールファイルを準備しています",
            ["StageRegister"] = "最終処理をしています",
            ["StageUnregister"] = "登録情報を削除しています",
            ["StageRemovePrevious"] = "以前のバージョンを削除しています",
            ["StageRollback"] = "変更をロールバックしています",
            ["PlanErrorFmt"] = "セットアップを準備できませんでした（エラー 0x{0:X8}）。",
        };

        private static readonly Dictionary<string, string> Korean = new Dictionary<string, string>
        {
            ["LanguageAuto"] = "자동",
            ["WindowTitle"] = "Mufpen 설치",
            ["Detecting"] = "설치된 버전 확인 중…",
            ["WelcomeTagline"] = "조용한 Markdown 작성 공간. 빠르고 네이티브이며 방해하지 않습니다.",
            ["Version"] = "버전 {0}",
            ["Options"] = "옵션",
            ["AddContextMenu"] = "탐색기 상황에 맞는 메뉴에 \"Mufpen으로 열기\" 추가",
            ["AssociateFiles"] = "Markdown 파일의 \"연결 프로그램\"에 Mufpen 표시",
            ["Install"] = "설치",
            ["Update"] = "업데이트",
            ["Repair"] = "복구",
            ["Uninstall"] = "제거",
            ["InstalledTitle"] = "Mufpen이 설치되어 있습니다",
            ["InstalledSub"] = "버전 {0}을(를) 사용할 수 있습니다.",
            ["UninstallTitle"] = "Mufpen을 제거할까요?",
            ["UninstallBody"] = "이 컴퓨터에서 Mufpen을 제거합니다. 메모, 문서, 개인 설정은 유지됩니다.",
            ["Keep"] = "유지",
            ["Installing"] = "Mufpen 설치 중…",
            ["Updating"] = "Mufpen 업데이트 중…",
            ["Repairing"] = "Mufpen 복구 중…",
            ["Removing"] = "Mufpen 제거 중…",
            ["Cancel"] = "취소",
            ["SuccessInstalled"] = "Mufpen 준비 완료",
            ["SuccessRemoved"] = "Mufpen이 제거되었습니다",
            ["RestartNote"] = "시스템 파일 업데이트를 마치려면 다시 시작하는 것이 좋습니다.",
            ["Launch"] = "Mufpen 시작",
            ["Close"] = "닫기",
            ["FailedTitle"] = "설치를 완료할 수 없습니다",
            ["OpenLog"] = "설치 로그 열기",
            ["NewerInstalled"] = "더 최신 버전의 Mufpen이 이미 설치되어 있습니다. 이 버전을 설치하려면 먼저 제거하세요.",
            ["ErrorFmt"] = "설치를 완료할 수 없습니다(오류 0x{0:X8}). 자세한 내용은 로그를 참고하세요.",
            ["StageCache"] = "설치 파일을 준비하는 중",
            ["StageRegister"] = "마무리하는 중",
            ["StageUnregister"] = "등록 정보를 제거하는 중",
            ["StageRemovePrevious"] = "이전 버전을 제거하는 중",
            ["StageRollback"] = "변경 내용을 되돌리는 중",
            ["PlanErrorFmt"] = "설치를 준비할 수 없습니다(오류 0x{0:X8}).",
        };

        private static readonly Dictionary<string, string> German = new Dictionary<string, string>
        {
            ["LanguageAuto"] = "Automatisch",
            ["WindowTitle"] = "Mufpen Setup",
            ["Detecting"] = "Installierte Versionen werden geprüft…",
            ["WelcomeTagline"] = "Ein ruhiger Ort für Markdown. Schnell, nativ, unaufdringlich.",
            ["Version"] = "Version {0}",
            ["Options"] = "OPTIONEN",
            ["AddContextMenu"] = "„Mit Mufpen öffnen“ zum Explorer-Kontextmenü hinzufügen",
            ["AssociateFiles"] = "Mufpen unter „Öffnen mit“ für Markdown-Dateien anzeigen",
            ["Install"] = "Installieren",
            ["Update"] = "Aktualisieren",
            ["Repair"] = "Reparieren",
            ["Uninstall"] = "Deinstallieren",
            ["InstalledTitle"] = "Mufpen ist installiert",
            ["InstalledSub"] = "Version {0} ist auf diesem Computer bereit.",
            ["UninstallTitle"] = "Mufpen deinstallieren?",
            ["UninstallBody"] = "Mufpen wird von diesem Computer entfernt. Deine Notizen, Dokumente und persönlichen Einstellungen bleiben erhalten.",
            ["Keep"] = "Behalten",
            ["Installing"] = "Mufpen wird installiert…",
            ["Updating"] = "Mufpen wird aktualisiert…",
            ["Repairing"] = "Mufpen wird repariert…",
            ["Removing"] = "Mufpen wird entfernt…",
            ["Cancel"] = "Abbrechen",
            ["SuccessInstalled"] = "Mufpen ist bereit",
            ["SuccessRemoved"] = "Mufpen wurde entfernt",
            ["RestartNote"] = "Ein Neustart wird empfohlen, um die Systemdateien zu aktualisieren.",
            ["Launch"] = "Mufpen starten",
            ["Close"] = "Schließen",
            ["FailedTitle"] = "Setup konnte nicht abgeschlossen werden",
            ["OpenLog"] = "Setup-Protokoll öffnen",
            ["NewerInstalled"] = "Es ist bereits eine neuere Version von Mufpen installiert. Deinstalliere diese zuerst, um diese Version zu installieren.",
            ["ErrorFmt"] = "Setup konnte nicht abgeschlossen werden (Fehler 0x{0:X8}). Details im Protokoll.",
            ["StageCache"] = "Installationsdateien werden vorbereitet",
            ["StageRegister"] = "Abschluss läuft",
            ["StageUnregister"] = "Registrierung wird entfernt",
            ["StageRemovePrevious"] = "Die vorherige Version wird entfernt",
            ["StageRollback"] = "Änderungen werden zurückgenommen",
            ["PlanErrorFmt"] = "Setup konnte nicht vorbereitet werden (Fehler 0x{0:X8}).",
        };

        private static readonly Dictionary<string, string> French = new Dictionary<string, string>
        {
            ["LanguageAuto"] = "Auto",
            ["WindowTitle"] = "Installation de Mufpen",
            ["Detecting"] = "Vérification des versions installées…",
            ["WelcomeTagline"] = "Un espace paisible pour Markdown. Rapide, natif et discret.",
            ["Version"] = "Version {0}",
            ["Options"] = "OPTIONS",
            ["AddContextMenu"] = "Ajouter « Ouvrir avec Mufpen » au menu contextuel de l'Explorateur",
            ["AssociateFiles"] = "Afficher Mufpen dans « Ouvrir avec » pour les fichiers Markdown",
            ["Install"] = "Installer",
            ["Update"] = "Mettre à jour",
            ["Repair"] = "Réparer",
            ["Uninstall"] = "Désinstaller",
            ["InstalledTitle"] = "Mufpen est installé",
            ["InstalledSub"] = "La version {0} est prête sur cet ordinateur.",
            ["UninstallTitle"] = "Désinstaller Mufpen ?",
            ["UninstallBody"] = "Mufpen sera supprimé de cet ordinateur. Vos notes, documents et paramètres personnels sont conservés.",
            ["Keep"] = "Conserver",
            ["Installing"] = "Installation de Mufpen…",
            ["Updating"] = "Mise à jour de Mufpen…",
            ["Repairing"] = "Réparation de Mufpen…",
            ["Removing"] = "Suppression de Mufpen…",
            ["Cancel"] = "Annuler",
            ["SuccessInstalled"] = "Mufpen est prêt",
            ["SuccessRemoved"] = "Mufpen a été supprimé",
            ["RestartNote"] = "Un redémarrage est recommandé pour terminer la mise à jour des fichiers système.",
            ["Launch"] = "Lancer Mufpen",
            ["Close"] = "Fermer",
            ["FailedTitle"] = "L'installation n'a pas pu aboutir",
            ["OpenLog"] = "Ouvrir le journal d'installation",
            ["NewerInstalled"] = "Une version plus récente de Mufpen est déjà installée. Désinstallez-la d'abord pour installer cette version.",
            ["ErrorFmt"] = "L'installation n'a pas pu aboutir (erreur 0x{0:X8}). Les détails sont dans le journal.",
            ["StageCache"] = "Préparation des fichiers d'installation",
            ["StageRegister"] = "Finalisation",
            ["StageUnregister"] = "Suppression de l'inscription",
            ["StageRemovePrevious"] = "Suppression de la version précédente",
            ["StageRollback"] = "Annulation des modifications",
            ["PlanErrorFmt"] = "Impossible de préparer l'installation (erreur 0x{0:X8}).",
        };

        private static readonly Dictionary<string, string> Spanish = new Dictionary<string, string>
        {
            ["LanguageAuto"] = "Auto",
            ["WindowTitle"] = "Instalación de Mufpen",
            ["Detecting"] = "Comprobando versiones instaladas…",
            ["WelcomeTagline"] = "Un lugar tranquilo para Markdown. Rápido, nativo y sin molestar.",
            ["Version"] = "Versión {0}",
            ["Options"] = "OPCIONES",
            ["AddContextMenu"] = "Añadir «Abrir con Mufpen» al menú contextual del Explorador",
            ["AssociateFiles"] = "Mostrar Mufpen en «Abrir con» para archivos Markdown",
            ["Install"] = "Instalar",
            ["Update"] = "Actualizar",
            ["Repair"] = "Reparar",
            ["Uninstall"] = "Desinstalar",
            ["InstalledTitle"] = "Mufpen está instalado",
            ["InstalledSub"] = "La versión {0} está lista en este equipo.",
            ["UninstallTitle"] = "¿Desinstalar Mufpen?",
            ["UninstallBody"] = "Mufpen se eliminará de este equipo. Tus notas, documentos y ajustes personales se conservan.",
            ["Keep"] = "Conservar",
            ["Installing"] = "Instalando Mufpen…",
            ["Updating"] = "Actualizando Mufpen…",
            ["Repairing"] = "Reparando Mufpen…",
            ["Removing"] = "Eliminando Mufpen…",
            ["Cancel"] = "Cancelar",
            ["SuccessInstalled"] = "Mufpen está listo",
            ["SuccessRemoved"] = "Mufpen se ha eliminado",
            ["RestartNote"] = "Se recomienda reiniciar para terminar de actualizar los archivos del sistema.",
            ["Launch"] = "Iniciar Mufpen",
            ["Close"] = "Cerrar",
            ["FailedTitle"] = "La instalación no pudo completarse",
            ["OpenLog"] = "Abrir registro de instalación",
            ["NewerInstalled"] = "Ya hay instalada una versión más reciente de Mufpen. Desinstálala primero si quieres instalar esta versión.",
            ["ErrorFmt"] = "La instalación no pudo completarse (error 0x{0:X8}). Los detalles están en el registro.",
            ["StageCache"] = "Preparando los archivos de instalación",
            ["StageRegister"] = "Finalizando",
            ["StageUnregister"] = "Quitando el registro",
            ["StageRemovePrevious"] = "Quitando la versión anterior",
            ["StageRollback"] = "Deshaciendo los cambios",
            ["PlanErrorFmt"] = "No se pudo preparar la instalación (error 0x{0:X8}).",
        };

        private static readonly Dictionary<string, string> Italian = new Dictionary<string, string>
        {
            ["LanguageAuto"] = "Auto",
            ["WindowTitle"] = "Installazione di Mufpen",
            ["Detecting"] = "Verifica delle versioni installate…",
            ["WelcomeTagline"] = "Un luogo tranquillo per Markdown. Veloce, nativo e discreto.",
            ["Version"] = "Versione {0}",
            ["Options"] = "OPZIONI",
            ["AddContextMenu"] = "Aggiungi «Apri con Mufpen» al menu contestuale di Esplora file",
            ["AssociateFiles"] = "Mostra Mufpen in «Apri con» per i file Markdown",
            ["Install"] = "Installa",
            ["Update"] = "Aggiorna",
            ["Repair"] = "Ripara",
            ["Uninstall"] = "Disinstalla",
            ["InstalledTitle"] = "Mufpen è installato",
            ["InstalledSub"] = "La versione {0} è pronta su questo computer.",
            ["UninstallTitle"] = "Disinstallare Mufpen?",
            ["UninstallBody"] = "Mufpen verrà rimosso da questo computer. Note, documenti e impostazioni personali verranno conservati.",
            ["Keep"] = "Mantieni",
            ["Installing"] = "Installazione di Mufpen…",
            ["Updating"] = "Aggiornamento di Mufpen…",
            ["Repairing"] = "Riparazione di Mufpen…",
            ["Removing"] = "Rimozione di Mufpen…",
            ["Cancel"] = "Annulla",
            ["SuccessInstalled"] = "Mufpen è pronto",
            ["SuccessRemoved"] = "Mufpen è stato rimosso",
            ["RestartNote"] = "È consigliabile riavviare per completare l'aggiornamento dei file di sistema.",
            ["Launch"] = "Avvia Mufpen",
            ["Close"] = "Chiudi",
            ["FailedTitle"] = "L'installazione non è stata completata",
            ["OpenLog"] = "Apri il registro dell'installazione",
            ["NewerInstalled"] = "È già installata una versione più recente di Mufpen. Disinstallala prima per installare questa versione.",
            ["ErrorFmt"] = "L'installazione non è stata completata (errore 0x{0:X8}). I dettagli sono nel registro.",
            ["StageCache"] = "Preparazione dei file di installazione",
            ["StageRegister"] = "Finalizzazione",
            ["StageUnregister"] = "Rimozione della registrazione",
            ["StageRemovePrevious"] = "Rimozione della versione precedente",
            ["StageRollback"] = "Ripristino delle modifiche",
            ["PlanErrorFmt"] = "Impossibile preparare l'installazione (errore 0x{0:X8}).",
        };

        private static readonly Dictionary<string, string> Dutch = new Dictionary<string, string>
        {
            ["LanguageAuto"] = "Auto",
            ["WindowTitle"] = "Mufpen-installatie",
            ["Detecting"] = "Geïnstalleerde versies worden gecontroleerd…",
            ["WelcomeTagline"] = "Een rustige plek voor Markdown. Snel, native en onopvallend.",
            ["Version"] = "Versie {0}",
            ["Options"] = "OPTIES",
            ["AddContextMenu"] = "„Openen met Mufpen“ toevoegen aan het contextmenu van Verkenner",
            ["AssociateFiles"] = "Mufpen tonen bij „Openen met“ voor Markdown-bestanden",
            ["Install"] = "Installeren",
            ["Update"] = "Bijwerken",
            ["Repair"] = "Herstellen",
            ["Uninstall"] = "Verwijderen",
            ["InstalledTitle"] = "Mufpen is geïnstalleerd",
            ["InstalledSub"] = "Versie {0} is klaar voor gebruik op deze computer.",
            ["UninstallTitle"] = "Mufpen verwijderen?",
            ["UninstallBody"] = "Mufpen wordt van deze computer verwijderd. Je notities, documenten en persoonlijke instellingen blijven bewaard.",
            ["Keep"] = "Behouden",
            ["Installing"] = "Mufpen wordt geïnstalleerd…",
            ["Updating"] = "Mufpen wordt bijgewerkt…",
            ["Repairing"] = "Mufpen wordt hersteld…",
            ["Removing"] = "Mufpen wordt verwijderd…",
            ["Cancel"] = "Annuleren",
            ["SuccessInstalled"] = "Mufpen is klaar",
            ["SuccessRemoved"] = "Mufpen is verwijderd",
            ["RestartNote"] = "Een herstart wordt aanbevolen om de systeembestanden bij te werken.",
            ["Launch"] = "Mufpen starten",
            ["Close"] = "Sluiten",
            ["FailedTitle"] = "De installatie is niet voltooid",
            ["OpenLog"] = "Installatielogboek openen",
            ["NewerInstalled"] = "Er is al een nieuwere versie van Mufpen geïnstalleerd. Verwijder deze eerst om deze versie te installeren.",
            ["ErrorFmt"] = "De installatie is niet voltooid (fout 0x{0:X8}). Zie het logboek voor details.",
            ["StageCache"] = "Installatiebestanden worden voorbereid",
            ["StageRegister"] = "Afhandeling",
            ["StageUnregister"] = "Registratie wordt verwijderd",
            ["StageRemovePrevious"] = "De vorige versie wordt verwijderd",
            ["StageRollback"] = "Wijzigingen worden teruggedraaid",
            ["PlanErrorFmt"] = "De installatie kon niet worden voorbereid (fout 0x{0:X8}).",
        };

        private static readonly Dictionary<string, string> Polish = new Dictionary<string, string>
        {
            ["LanguageAuto"] = "Auto",
            ["WindowTitle"] = "Instalator Mufpen",
            ["Detecting"] = "Sprawdzanie zainstalowanych wersji…",
            ["WelcomeTagline"] = "Spokojne miejsce dla Markdown. Szybki, natywny i nienachalny.",
            ["Version"] = "Wersja {0}",
            ["Options"] = "OPCJE",
            ["AddContextMenu"] = "Dodaj „Otwórz za pomocą Mufpen” do menu kontekstowego Eksploratora",
            ["AssociateFiles"] = "Pokazuj Mufpen w „Otwórz za pomocą” dla plików Markdown",
            ["Install"] = "Zainstaluj",
            ["Update"] = "Aktualizuj",
            ["Repair"] = "Napraw",
            ["Uninstall"] = "Odinstaluj",
            ["InstalledTitle"] = "Mufpen jest zainstalowany",
            ["InstalledSub"] = "Wersja {0} jest gotowa na tym komputerze.",
            ["UninstallTitle"] = "Odinstalować Mufpen?",
            ["UninstallBody"] = "Mufpen zostanie usunięty z tego komputera. Twoje notatki, dokumenty i ustawienia osobiste zostaną zachowane.",
            ["Keep"] = "Zachowaj",
            ["Installing"] = "Instalowanie Mufpen…",
            ["Updating"] = "Aktualizowanie Mufpen…",
            ["Repairing"] = "Naprawianie Mufpen…",
            ["Removing"] = "Usuwanie Mufpen…",
            ["Cancel"] = "Anuluj",
            ["SuccessInstalled"] = "Mufpen jest gotowy",
            ["SuccessRemoved"] = "Mufpen został usunięty",
            ["RestartNote"] = "Zalecany jest ponowny rozruch, aby zakończyć aktualizację plików systemowych.",
            ["Launch"] = "Uruchom Mufpen",
            ["Close"] = "Zamknij",
            ["FailedTitle"] = "Instalacja nie została ukończona",
            ["OpenLog"] = "Otwórz dziennik instalacji",
            ["NewerInstalled"] = "Nowsza wersja Mufpen jest już zainstalowana. Odinstaluj ją najpierw, aby zainstalować tę wersję.",
            ["ErrorFmt"] = "Instalacja nie została ukończona (błąd 0x{0:X8}). Szczegóły w dzienniku.",
            ["StageCache"] = "Przygotowywanie plików instalacji",
            ["StageRegister"] = "Kończenie",
            ["StageUnregister"] = "Usuwanie rejestracji",
            ["StageRemovePrevious"] = "Usuwanie poprzedniej wersji",
            ["StageRollback"] = "Cofanie zmian",
            ["PlanErrorFmt"] = "Nie można przygotować instalacji (błąd 0x{0:X8}).",
        };

        private static readonly Dictionary<string, string> PortugueseBrazil = new Dictionary<string, string>
        {
            ["LanguageAuto"] = "Auto",
            ["WindowTitle"] = "Instalação do Mufpen",
            ["Detecting"] = "Verificando versões instaladas…",
            ["WelcomeTagline"] = "Um lugar tranquilo para Markdown. Rápido, nativo e discreto.",
            ["Version"] = "Versão {0}",
            ["Options"] = "OPÇÕES",
            ["AddContextMenu"] = "Adicionar \"Abrir com Mufpen\" ao menu de contexto do Explorador",
            ["AssociateFiles"] = "Mostrar Mufpen em \"Abrir com\" para arquivos Markdown",
            ["Install"] = "Instalar",
            ["Update"] = "Atualizar",
            ["Repair"] = "Reparar",
            ["Uninstall"] = "Desinstalar",
            ["InstalledTitle"] = "Mufpen está instalado",
            ["InstalledSub"] = "A versão {0} está pronta neste computador.",
            ["UninstallTitle"] = "Desinstalar o Mufpen?",
            ["UninstallBody"] = "O Mufpen será removido deste computador. Suas notas, documentos e configurações pessoais serão mantidos.",
            ["Keep"] = "Manter",
            ["Installing"] = "Instalando o Mufpen…",
            ["Updating"] = "Atualizando o Mufpen…",
            ["Repairing"] = "Reparando o Mufpen…",
            ["Removing"] = "Removendo o Mufpen…",
            ["Cancel"] = "Cancelar",
            ["SuccessInstalled"] = "Mufpen está pronto",
            ["SuccessRemoved"] = "Mufpen foi removido",
            ["RestartNote"] = "Recomenda-se reiniciar para concluir a atualização dos arquivos do sistema.",
            ["Launch"] = "Iniciar Mufpen",
            ["Close"] = "Fechar",
            ["FailedTitle"] = "A instalação não pôde ser concluída",
            ["OpenLog"] = "Abrir log de instalação",
            ["NewerInstalled"] = "Uma versão mais recente do Mufpen já está instalada. Desinstale-a primeiro para instalar esta versão.",
            ["ErrorFmt"] = "A instalação não pôde ser concluída (erro 0x{0:X8}). Detalhes no log.",
            ["StageCache"] = "Preparando os arquivos de instalação",
            ["StageRegister"] = "Finalizando",
            ["StageUnregister"] = "Removendo o registro",
            ["StageRemovePrevious"] = "Removendo a versão anterior",
            ["StageRollback"] = "Revertendo as alterações",
            ["PlanErrorFmt"] = "Não foi possível preparar a instalação (erro 0x{0:X8}).",
        };

        private static readonly Dictionary<string, string> Russian = new Dictionary<string, string>
        {
            ["LanguageAuto"] = "Авто",
            ["WindowTitle"] = "Установка Mufpen",
            ["Detecting"] = "Проверка установленных версий…",
            ["WelcomeTagline"] = "Спокойное место для Markdown. Быстро, нативно, ненавязчиво.",
            ["Version"] = "Версия {0}",
            ["Options"] = "ПАРАМЕТРЫ",
            ["AddContextMenu"] = "Добавить «Открыть в Mufpen» в контекстное меню проводника",
            ["AssociateFiles"] = "Показывать Mufpen в «Открыть с помощью» для файлов Markdown",
            ["Install"] = "Установить",
            ["Update"] = "Обновить",
            ["Repair"] = "Восстановить",
            ["Uninstall"] = "Удалить",
            ["InstalledTitle"] = "Mufpen установлен",
            ["InstalledSub"] = "Версия {0} готова на этом компьютере.",
            ["UninstallTitle"] = "Удалить Mufpen?",
            ["UninstallBody"] = "Mufpen будет удалён с этого компьютера. Ваши заметки, документы и личные настройки сохранятся.",
            ["Keep"] = "Оставить",
            ["Installing"] = "Установка Mufpen…",
            ["Updating"] = "Обновление Mufpen…",
            ["Repairing"] = "Восстановление Mufpen…",
            ["Removing"] = "Удаление Mufpen…",
            ["Cancel"] = "Отмена",
            ["SuccessInstalled"] = "Mufpen готов к работе",
            ["SuccessRemoved"] = "Mufpen удалён",
            ["RestartNote"] = "Рекомендуется перезагрузить компьютер, чтобы завершить обновление системных файлов.",
            ["Launch"] = "Запустить Mufpen",
            ["Close"] = "Закрыть",
            ["FailedTitle"] = "Не удалось завершить установку",
            ["OpenLog"] = "Открыть журнал установки",
            ["NewerInstalled"] = "Более новая версия Mufpen уже установлена. Сначала удалите её, чтобы установить эту версию.",
            ["ErrorFmt"] = "Не удалось завершить установку (ошибка 0x{0:X8}). Подробности — в журнале.",
            ["StageCache"] = "Подготовка файлов установки",
            ["StageRegister"] = "Завершение",
            ["StageUnregister"] = "Удаление регистрации",
            ["StageRemovePrevious"] = "Удаление предыдущей версии",
            ["StageRollback"] = "Откат изменений",
            ["PlanErrorFmt"] = "Не удалось подготовить установку (ошибка 0x{0:X8}).",
        };

        private static readonly Dictionary<string, string> Turkish = new Dictionary<string, string>
        {
            ["LanguageAuto"] = "Otomatik",
            ["WindowTitle"] = "Mufpen Kurulumu",
            ["Detecting"] = "Yüklü sürümler denetleniyor…",
            ["WelcomeTagline"] = "Markdown için sakin bir alan. Hızlı, yerel ve rahatsız etmeyen.",
            ["Version"] = "Sürüm {0}",
            ["Options"] = "SEÇENEKLER",
            ["AddContextMenu"] = "Gezgin sağ tık menüsüne \"Mufpen ile aç\" ekle",
            ["AssociateFiles"] = "Markdown dosyaları için \"Birlikte aç\"ta Mufpen'i göster",
            ["Install"] = "Yükle",
            ["Update"] = "Güncelle",
            ["Repair"] = "Onar",
            ["Uninstall"] = "Kaldır",
            ["InstalledTitle"] = "Mufpen yüklü",
            ["InstalledSub"] = "Sürüm {0} bu bilgisayarda hazır.",
            ["UninstallTitle"] = "Mufpen kaldırılsın mı?",
            ["UninstallBody"] = "Mufpen bu bilgisayardan kaldırılacak. Notlarınız, belgeleriniz ve kişisel ayarlarınız saklanır.",
            ["Keep"] = "Koru",
            ["Installing"] = "Mufpen yükleniyor…",
            ["Updating"] = "Mufpen güncelleniyor…",
            ["Repairing"] = "Mufpen onarılıyor…",
            ["Removing"] = "Mufpen kaldırılıyor…",
            ["Cancel"] = "İptal",
            ["SuccessInstalled"] = "Mufpen hazır",
            ["SuccessRemoved"] = "Mufpen kaldırıldı",
            ["RestartNote"] = "Sistem dosyalarının güncellenmesini tamamlamak için yeniden başlatma önerilir.",
            ["Launch"] = "Mufpen'i başlat",
            ["Close"] = "Kapat",
            ["FailedTitle"] = "Kurulum tamamlanamadı",
            ["OpenLog"] = "Kurulum günlüğünü aç",
            ["NewerInstalled"] = "Daha yeni bir Mufpen sürümü zaten yüklü. Bu sürümü yüklemek için önce onu kaldırın.",
            ["ErrorFmt"] = "Kurulum tamamlanamadı (hata 0x{0:X8}). Ayrıntılar günlükte.",
            ["StageCache"] = "Kurulum dosyaları hazırlanıyor",
            ["StageRegister"] = "Sonlandırılıyor",
            ["StageUnregister"] = "Kayıt kaldırılıyor",
            ["StageRemovePrevious"] = "Önceki sürüm kaldırılıyor",
            ["StageRollback"] = "Değişiklikler geri alınıyor",
            ["PlanErrorFmt"] = "Kurulum hazırlanamadı (hata 0x{0:X8}).",
        };

        private static readonly Dictionary<string, string> Vietnamese = new Dictionary<string, string>
        {
            ["LanguageAuto"] = "Tự động",
            ["WindowTitle"] = "Cài đặt Mufpen",
            ["Detecting"] = "Đang kiểm tra các phiên bản đã cài đặt…",
            ["WelcomeTagline"] = "Một không gian yên tĩnh cho Markdown. Nhanh, gốc và không làm phiền.",
            ["Version"] = "Phiên bản {0}",
            ["Options"] = "TÙY CHỌN",
            ["AddContextMenu"] = "Thêm \"Mở bằng Mufpen\" vào menu chuột phải của Explorer",
            ["AssociateFiles"] = "Hiển thị Mufpen trong \"Mở bằng\" cho các tệp Markdown",
            ["Install"] = "Cài đặt",
            ["Update"] = "Cập nhật",
            ["Repair"] = "Sửa chữa",
            ["Uninstall"] = "Gỡ cài đặt",
            ["InstalledTitle"] = "Mufpen đã được cài đặt",
            ["InstalledSub"] = "Phiên bản {0} đã sẵn sàng trên máy tính này.",
            ["UninstallTitle"] = "Gỡ cài đặt Mufpen?",
            ["UninstallBody"] = "Mufpen sẽ bị xóa khỏi máy tính này. Ghi chú, tài liệu và cài đặt cá nhân của bạn được giữ lại.",
            ["Keep"] = "Giữ lại",
            ["Installing"] = "Đang cài đặt Mufpen…",
            ["Updating"] = "Đang cập nhật Mufpen…",
            ["Repairing"] = "Đang sửa chữa Mufpen…",
            ["Removing"] = "Đang xóa Mufpen…",
            ["Cancel"] = "Hủy",
            ["SuccessInstalled"] = "Mufpen đã sẵn sàng",
            ["SuccessRemoved"] = "Đã xóa Mufpen",
            ["RestartNote"] = "Nên khởi động lại để hoàn tất cập nhật tệp hệ thống.",
            ["Launch"] = "Khởi động Mufpen",
            ["Close"] = "Đóng",
            ["FailedTitle"] = "Không thể hoàn tất cài đặt",
            ["OpenLog"] = "Mở nhật ký cài đặt",
            ["NewerInstalled"] = "Đã cài đặt phiên bản Mufpen mới hơn. Hãy gỡ nó trước nếu muốn cài phiên bản này.",
            ["ErrorFmt"] = "Không thể hoàn tất cài đặt (lỗi 0x{0:X8}). Chi tiết trong nhật ký.",
            ["StageCache"] = "Đang chuẩn bị tệp cài đặt",
            ["StageRegister"] = "Đang hoàn tất",
            ["StageUnregister"] = "Đang xóa đăng ký",
            ["StageRemovePrevious"] = "Đang xóa phiên bản trước",
            ["StageRollback"] = "Đang hoàn tác thay đổi",
            ["PlanErrorFmt"] = "Không thể chuẩn bị cài đặt (lỗi 0x{0:X8}).",
        };

        private static readonly Dictionary<string, Dictionary<string, string>> Tables =
            new Dictionary<string, Dictionary<string, string>>
        {
            ["en"] = English,
            ["zh-CN"] = ChineseSimplified,
            ["zh-TW"] = ChineseTraditional,
            ["ja"] = Japanese,
            ["ko"] = Korean,
            ["de"] = German,
            ["fr"] = French,
            ["es"] = Spanish,
            ["it"] = Italian,
            ["nl"] = Dutch,
            ["pl"] = Polish,
            ["pt-BR"] = PortugueseBrazil,
            ["ru"] = Russian,
            ["tr"] = Turkish,
            ["vi"] = Vietnamese,
        };

        static UiStrings()
        {
            var labels = new[] {
                "Clear personal settings for the current user", "清除当前用户的个人设置", "清除目前使用者的個人設定",
                "現在のユーザーの個人設定を削除", "현재 사용자의 개인 설정 삭제", "Persönliche Einstellungen des aktuellen Benutzers löschen",
                "Effacer les paramètres personnels de l’utilisateur actuel", "Borrar los ajustes personales del usuario actual",
                "Cancella le impostazioni personali dell’utente corrente", "Persoonlijke instellingen van de huidige gebruiker wissen",
                "Usuń ustawienia osobiste bieżącego użytkownika", "Limpar as configurações pessoais do usuário atual",
                "Удалить личные настройки текущего пользователя", "Geçerli kullanıcının kişisel ayarlarını sil",
                "Xóa cài đặt cá nhân của người dùng hiện tại"
            };
            var bodies = new[] {
                "Mufpen and your personal settings will be removed. Notes, documents, recovery drafts, themes and replacement backups are kept.",
                "将移除 Mufpen 并清除个人设置。笔记、文档、恢复草稿、主题和替换备份会保留。",
                "將移除 Mufpen 並清除個人設定。筆記、文件、復原草稿、主題和取代備份會保留。",
                "Mufpen と個人設定を削除します。ノート、文書、復元用下書き、テーマ、置換バックアップは保持されます。",
                "Mufpen과 개인 설정을 삭제합니다. 메모, 문서, 복구 초안, 테마 및 바꾸기 백업은 유지됩니다.",
                "Mufpen und deine persönlichen Einstellungen werden entfernt. Notizen, Dokumente, Wiederherstellungsentwürfe, Themes und Ersetzungssicherungen bleiben erhalten.",
                "Mufpen et vos paramètres personnels seront supprimés. Notes, documents, brouillons de récupération, thèmes et sauvegardes de remplacement sont conservés.",
                "Se eliminarán Mufpen y tus ajustes personales. Se conservarán las notas, documentos, borradores de recuperación, temas y copias de seguridad de reemplazos.",
                "Mufpen e le impostazioni personali verranno rimossi. Note, documenti, bozze di recupero, temi e backup delle sostituzioni verranno conservati.",
                "Mufpen en je persoonlijke instellingen worden verwijderd. Notities, documenten, herstelconcepten, thema’s en vervangingsback-ups blijven behouden.",
                "Mufpen i ustawienia osobiste zostaną usunięte. Notatki, dokumenty, szkice odzyskiwania, motywy i kopie zapasowe zamian zostaną zachowane.",
                "O Mufpen e suas configurações pessoais serão removidos. Notas, documentos, rascunhos de recuperação, temas e backups de substituições serão mantidos.",
                "Mufpen и личные настройки будут удалены. Заметки, документы, черновики восстановления, темы и резервные копии замен сохранятся.",
                "Mufpen ve kişisel ayarlarınız kaldırılacak. Notlar, belgeler, kurtarma taslakları, temalar ve değiştirme yedekleri korunacak.",
                "Mufpen và cài đặt cá nhân sẽ bị xóa. Ghi chú, tài liệu, bản nháp khôi phục, chủ đề và bản sao lưu thay thế được giữ lại."
            };
            var warnings = new[] {
                "Mufpen was removed, but its settings could not be cleared. Close running Mufpen instances and see the setup log.",
                "Mufpen 已移除，但未能清除设置。请关闭仍在运行的 Mufpen 实例，并查看安装日志。",
                "Mufpen 已移除，但無法清除設定。請關閉仍在執行的 Mufpen 執行個體，並查看安裝記錄。",
                "Mufpen は削除されましたが、設定を削除できませんでした。実行中の Mufpen を終了し、セットアップログをご確認ください。",
                "Mufpen은 제거되었지만 설정을 지우지 못했습니다. 실행 중인 Mufpen을 닫고 설치 로그를 확인하세요.",
                "Mufpen wurde entfernt, die Einstellungen konnten jedoch nicht gelöscht werden. Schließe laufende Mufpen-Instanzen und prüfe das Installationsprotokoll.",
                "Mufpen a été supprimé, mais ses paramètres n’ont pas pu être effacés. Fermez les instances de Mufpen et consultez le journal d’installation.",
                "Mufpen se eliminó, pero no se pudieron borrar sus ajustes. Cierra las instancias de Mufpen y consulta el registro de instalación.",
                "Mufpen è stato rimosso, ma non è stato possibile cancellare le impostazioni. Chiudi le istanze di Mufpen e consulta il registro di installazione.",
                "Mufpen is verwijderd, maar de instellingen konden niet worden gewist. Sluit actieve Mufpen-vensters en bekijk het installatielogboek.",
                "Mufpen został usunięty, ale nie udało się wyczyścić ustawień. Zamknij uruchomione instancje Mufpen i sprawdź dziennik instalacji.",
                "O Mufpen foi removido, mas não foi possível limpar as configurações. Feche as instâncias do Mufpen e consulte o log de instalação.",
                "Mufpen удалён, но очистить настройки не удалось. Закройте запущенные экземпляры Mufpen и проверьте журнал установки.",
                "Mufpen kaldırıldı ancak ayarları silinemedi. Çalışan Mufpen örneklerini kapatıp kurulum günlüğünü inceleyin.",
                "Đã xóa Mufpen nhưng không thể xóa cài đặt. Hãy đóng các phiên Mufpen đang chạy và xem nhật ký cài đặt."
            };
            for (var i = 0; i < LanguageCodes.Length; ++i)
            {
                Tables[LanguageCodes[i]]["RemovePersonalSettings"] = labels[i];
                Tables[LanguageCodes[i]]["UninstallCleanupBody"] = bodies[i];
                Tables[LanguageCodes[i]]["CleanupWarning"] = warnings[i];
            }
        }

        /// <summary>"auto" (follow system) or an entry of LanguageCodes.</summary>
        private static string _selected = "auto";

        private static Dictionary<string, string> _table = English;

        /// <summary>Resolve the startup language from the OS UI culture.</summary>
        public static void Initialize()
        {
            _selected = "auto";
            Resolve();
        }

        /// <summary>Switch explicitly ("auto" re-resolves against the OS culture).</summary>
        public static void SetLanguage(string code)
        {
            if (!Tables.ContainsKey(code))
            {
                code = "auto";
            }
            _selected = code;
            Resolve();
        }

        private static void Resolve()
        {
            if (_selected != "auto")
            {
                _table = Tables[_selected];
                return;
            }

            var name = CultureInfo.CurrentUICulture.Name;
            if (string.IsNullOrEmpty(name))
            {
                _table = English;
                return;
            }

            // Traditional Chinese (incl. HK/MO) and bare "zh" (mainland default)
            // fall back on opposite sides of the strait.
            if (name.StartsWith("zh-TW") || name.StartsWith("zh-HK") || name.StartsWith("zh-MO") || name.StartsWith("zh-Hant"))
            {
                _table = Tables["zh-TW"];
            }
            else if (name.StartsWith("zh"))
            {
                _table = Tables["zh-CN"];
            }
            else if (name.StartsWith("pt"))
            {
                _table = Tables["pt-BR"];
            }
            else
            {
                var twoLetter = CultureInfo.CurrentUICulture.TwoLetterISOLanguageName;
                _table = Tables.TryGetValue(twoLetter, out var table) ? table : English;
            }
        }

        public static string Get(string key)
        {
            if (_table.TryGetValue(key, out var value))
            {
                return value;
            }
            return English.TryGetValue(key, out var fallback) ? fallback : key;
        }
    }
}

<?hh // strict

namespace Zynga\Framework\IO\Disk\V1\Interfaces;

/**
 * Interface for managing Disk IO
 */
interface ManagerInterface {

  /**
   * Creates a folder at the given path if none exists.
   *
   * @param $path Absolute path of directory to create
   * @param $permissions Integer flag representing file permissions
   * @return bool True if the path already exists or was successfully created,
   *         otherwise false
   *
   */
  public function checkOrCreatePath(string $path, int $permissions): bool;

  /**
   * Deletes the file at the given path if one exists.
   *
   * @param $path Absolute path of directory to create
   * @return bool True if the file does not exist or was successfully deleted,
   *         otherwise false
   */
  public function deleteFile(string $path): bool;

  /**
   * Creates (if neccessary) a file at the given path and writes the provided
   * data to it.
   *
   * @param $fileName Absolute path of file to create/write to
   * @param $dataToWrite Data to write to file
   * @param $permissions Integer flag representing file permissions
   * @param $appendIfExists If true, appends to the file if it already exists.
   *        Otherwise, overwrites the existing file.
   * @throws Zynga\Framework\IO\Disk\V1\Exception\FailedToCreateDirectoryException
   * @throws Zynga\Framework\IO\Disk\V1\Exception\FailedToOpenFileException
   * @throws Zynga\Framework\IO\Disk\V1\Exception\FailedToWriteToFileException
   * @throws Zynga\Framework\IO\Disk\V1\Exception\FailedToCloseFileException
   */
  public function writeFile(
    string $fileName,
    string $dataToWrite,
    int $permissions,
    bool $appendIfExists,
  ): void;

  /**
   * Compresses the given file into a bzip2 file at the given output path.
   *
   * @param $in Absolute path of file to compress
   * @param $out Absolute path of output bzip2 file
   * @throws Zynga\Framework\IO\Disk\V1\Exception\ReadPermissionsException
   * @throws Zynga\Framework\IO\Disk\V1\Exception\WritePermissionsException
   * @throws Zynga\Framework\IO\Disk\V1\Exception\FailedToOpenFileException
   * @throws Zynga\Framework\IO\Disk\V1\Exception\FailedToCloseFileException
   */
  public function bzip2(string $in, string $out): void;

  /**
   * Modifies the given file to be owned by the given user, if possible.
   *
   * @param $fileName Absolute path of file changing ownership
   * @param $userName Name of user to set as owner of file
   * @return True on success, otherwise false
   */
  public function chown(string $fileName, string $userName): bool;

  /**
   * Deletes the given file path
   *
   * @param $path Absolute path of directory to be deleted
   * @throws Zynga\Framework\IO\Disk\V1\Exception\FailedToDeleteFileException
   */
  public function deleteDirectory(string $path): void;

  /**
   * Recursively deletes the given directory and all of its contents
   *
   * @param $path Absolute path of directory to be deleted
   * @param $minimumSecondsSinceModificaiton Only delete files and folders
   *        modified >= this many seconds ago
   * @return Number of files (directories included) that were successfully deleted
   */
  public function recursivelyDeleteDirectory(
    string $path,
    int $minimumSecondsSinceModification = 0,
  ): int;

  /**
   * Given a file or directory path as input, create a tarball file at the given output
   *
   * @param $in Absolute path of file/directory to tar
   * @param $out Absolute path of created tarball
   * @throws ReadPermissionsException
   * @throws WritePermissionsException
   * @throws InvalidFileNameException
   * @throws FailedToWriteToFileException
   */
  public function tarball(string $in, string $out): void;

  /**
   *
   */
  public function isDirectory(string $path): bool;
  public function feof(resource $handle): bool;
  public function fgets(resource $handle, int $maxBytesToRead): string;
  public function doesFileExist(string $fileName): bool;
  public function fileOpen(string $fileName, string $mode): mixed;
  public function directoryName(string $fileName): string;
  public function makeDirectory(
    string $path,
    int $permissions,
    bool $recursivePermissions,
  ): bool;
  public function unlink(string $fileName): bool;
  public function fwrite(resource $handle, string $dataToWrite): mixed;
  public function fclose(resource $handle): bool;
  public function isReadable(string $fileName): bool;
  public function isWriteable(string $fileName): bool;
  public function bzopen(string $fileName, string $mode): mixed;
  public function bzclose(resource $handle): bool;
  public function bzwrite(
    resource $handle,
    string $dataToWrite,
    int $maxBytesToRead,
  ): int;

  public function rmdir(string $path): bool;

  public function scanDirectory(string $path): Vector<resource>;

  /**
   * Test a given filename for validity as a tarball.
   *
   * @return bool $validTarball
   */
  public function isTarballValid(string $filename): bool;

}

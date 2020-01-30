<?hh // strict

namespace Zynga\Framework\PgData\V1\PgModel;

use Zynga\Framework\Exception\V1\Exception;
use Zynga\Framework\Lockable\Cache\V1\Factory as LockableCacheFactory;
use Zynga\Framework\Logging\V1\StaticLogger;
use
  Zynga\Framework\Lockable\Cache\V1\Interfaces\DriverInterface as LockableDriverInterface
;
use Zynga\Framework\PgData\V1\Interfaces\PgModelInterface;
use Zynga\Framework\PgData\V1\Interfaces\PgModel\CacheInterface;
use Zynga\Framework\PgData\V1\Interfaces\PgRowInterface;
use Zynga\Framework\PgData\V1\Interfaces\PgWhereClauseInterface;
use Zynga\Framework\PgData\V1\PgWriterOverride;

class Cache implements CacheInterface {
  const string WRITER_OVERRIDE_SUFFIX = ':useWriter';
  const int WRITER_OVERRIDE_TTL = 1;

  private PgModelInterface $_pgModel;

  public function __construct(PgModelInterface $pgModel) {
    $this->_pgModel = $pgModel;
  }

  private function pgModel(): PgModelInterface {
    return $this->_pgModel;
  }

  public function getDataCache(): LockableDriverInterface {
    try {
      $pgModel = $this->pgModel();
      return LockableCacheFactory::factory(
        LockableDriverInterface::class,
        $pgModel->getDataCacheName(),
      );
    } catch (Exception $e) {
      throw $e;
    }
  }

  public function getResultSetCache(): LockableDriverInterface {
    try {
      $pgModel = $this->pgModel();
      return LockableCacheFactory::factory(
        LockableDriverInterface::class,
        $pgModel->getResultSetCacheName(),
      );
    } catch (Exception $e) {
      throw $e;
    }
  }

  public function lockRowCache(PgRowInterface $row): bool {

    try {
      if($row->getPrimaryKeyTyped()->isDefaultValue()[0] === true) {
        throw new Exception("Trying to lock a pg row without primary key! Row=" . $row->export()->asJSON());
      }
      
      $cache = $this->getDataCache();
      
      return $cache->lock($row);

    } catch (Exception $e) {
      throw $e;
    }
  }

  public function unlockRowCache(PgRowInterface $row): bool {

    try {
      $cache = $this->getDataCache();

      return $cache->unlock($row);

    } catch (Exception $e) {
      throw $e;
    }

  }

  /**
   * API to force clear the result set cache so it can refetched.
   * Used by external systems to manage result set caches
   */
  public function clearResultSetCache<TModelClass as PgRowInterface>(
    classname<TModelClass> $model,
    PgWhereClauseInterface $where,
  ): bool {
    try {

      $pgModel = $this->pgModel();

      // Create a cachedRs out of the result set that was given.
      $cachedRs = $pgModel->reader()->createCachedResultSet($model, $where);

      $cache = $this->getResultSetCache();

      if ($cache->lock($cachedRs)) {
        $result = $cache->delete($cachedRs, false);
        if ($result && $pgModel->allowWriterOnClearingResultSetCache()) {
          if (!$this->setWriterOverrideForWhereClause($model, $where)) {
            StaticLogger::error(
              "PGData: Failed to save the Writer Overide Key.",
              Map {},
            );
          }
        }
        $cache->unlock($cachedRs);
        return $result;
      }

      return false;
    } catch (Exception $e) {
      throw $e;
    }
  }

  public function doesWriterOverrideKeyExist<TModelClass as PgRowInterface>(
    classname<TModelClass> $model,
    PgWhereClauseInterface $where,
  ): bool {
    try {
      $pgModel = $this->pgModel();
      if (!$pgModel->allowWriterOnClearingResultSetCache()) {
        return false;
      }

      $cachedRs = $pgModel->reader()->createCachedResultSet($model, $where);

      $cache = $this->getResultSetCache()->getConfig()->getCache();

      $key = $cachedRs->createChecksum().self::WRITER_OVERRIDE_SUFFIX;
      $writerOverride = $cache->get(new PgWriterOverride(), $key);
      return $writerOverride instanceof PgWriterOverride && $writerOverride->override->get() === true;
    } catch (Exception $e) {
      StaticLogger::exception(
        "PGData: Exception raise when determining if Writer Override key exists.",
        Map {},
        $e,
      );
    }
    return false;
  }

  /**
   * Used internally by the system to lock result set cache before populating data.
   * Should not be used by external systems.
   */
  public function lockResultSetCache<TModelClass as PgRowInterface>(
    classname<TModelClass> $model,
    PgWhereClauseInterface $where,
  ): bool {
    try {
      $pgModel = $this->pgModel();

      // Create a cachedRs out of the result set that was given.
      $cachedRs = $pgModel->reader()->createCachedResultSet($model, $where);

      $cache = $this->getResultSetCache();

      return $cache->lock($cachedRs);

    } catch (Exception $e) {
      throw $e;
    }
  }

  /**
   * Used internally by the system to unlock result set cache after populating data.
   * Should not be used by external systems.
   */
  public function unlockResultSetCache<TModelClass as PgRowInterface>(
    classname<TModelClass> $model,
    PgWhereClauseInterface $where,
  ): bool {
    try {
      $pgModel = $this->pgModel();

      // Create a cachedRs out of the result set that was given.
      $cachedRs = $pgModel->reader()->createCachedResultSet($model, $where);

      $cache = $this->getResultSetCache();

      return $cache->unlock($cachedRs);

    } catch (Exception $e) {
      throw $e;
    }
  }

  private function setWriterOverrideForWhereClause<TModelClass as PgRowInterface>(
    classname<TModelClass> $model,
    PgWhereClauseInterface $where,
  ): bool {
    try {
      $pgModel = $this->pgModel();

      $cachedRs = $pgModel->reader()->createCachedResultSet($model, $where);

      $cache = $this->getResultSetCache()->getConfig()->getCache();
      $key = $cachedRs->createChecksum().self::WRITER_OVERRIDE_SUFFIX;

      return $cache->set(new PgWriterOverride(), $key, self::WRITER_OVERRIDE_TTL);
    } catch (Exception $e) {
      StaticLogger::exception(
        "PGData: Exception raise when creating a Writer Override key.",
        Map {},
        $e,
      );
    }
    return false;
  }
}

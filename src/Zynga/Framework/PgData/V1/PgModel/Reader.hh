<?hh // strict

namespace Zynga\Framework\PgData\V1\PgModel;

use Zynga\Framework\Exception\V1\Exception;
use Zynga\Framework\PgData\V1\Interfaces\PgModelInterface;
use Zynga\Framework\PgData\V1\Interfaces\PgRowInterface;
use Zynga\Framework\PgData\V1\PgModel;
use Zynga\Framework\PgData\V1\PgModel\Reader\GetById;
use Zynga\Framework\PgData\V1\PgResultSet;
use Zynga\Framework\PgData\V1\SqlGenerator;
use Zynga\Framework\PgData\V1\WhereClause;
use Zynga\Framework\PgData\V1\WhereOperand;

class Reader {

  private PgModelInterface $_pgModel;

  public function __construct(PgModelInterface $pgModel) {
    $this->_pgModel = $pgModel;
  }

  private function pgModel(): PgModelInterface {
    return $this->_pgModel;
  }

  private function createSql(PgRowInterface $row, WhereClause $where): string {
    try {
      $pgModel = $this->pgModel();
      return SqlGenerator::getSelectSql(
        $pgModel->db()->getReadDatabase(),
        $pgModel,
        $row,
        $where,
      );
    } catch (Exception $e) {
      throw $e;
    }
  }

  public function getById<TModelClass as PgRowInterface>(
    classname<TModelClass> $model,
    mixed $id,
    bool $getLocked = false,
  ): ?PgRowInterface {

    try {

      // Snag a reference to the model.
      $pgModel = $this->pgModel();

      // 1) grab a copy of our object to work with.
      $obj = $pgModel->data()->createRowObjectFromClassName($model);

      // 2) Get a cached version of the object if possible.

      $pk = $obj->getPrimaryKeyTyped();
      $pk->set($id);

      $cache = $pgModel->cache()->getDataCache();
      $cachedObj = $cache->get($obj, $getLocked); // This will apply the lock if asked for.

      if ($cachedObj instanceof PgRowInterface) {
        $pgModel->stats()->incrementCacheHits();
        return $cachedObj;
      }

      $pgModel->stats()->incrementCacheMisses();

      // 3) Create sql for the ask to the db.

      // apply a lock while we deal with sql / databases.
      $cache->lock($obj);

      $where = new WhereClause();
      $where->and($obj->getPrimaryKey(), WhereOperand::EQUALS, $id);

      $sql = $this->createSql($obj, $where);

      // 4) Get a database handle.
      $dbh = $pgModel->db()->getReadDatabase();

      // 5) Run the query against the database.
      $sth = $dbh->query($sql);

      $pgModel->stats()->incrementSqlSelects();

      $wasSuccessful = $sth->wasSuccessful();
      $rowCount = $sth->getNumRows();

      error_log(
        'query wasSuccessful='.
        var_export($wasSuccessful, true).
        ' rowCount='.
        var_export($rowCount, true),
      );

      if ($wasSuccessful == true && $rowCount == 1) {

        $rawRow = $sth->fetchMap();
        error_log('attemptToHydrateRow='.var_export($rawRow, true));
        $pgModel->data()->hydrateDataToRowObject($obj, $rawRow);

        $cache->set($obj);

        // explicitly unlock
        if ($getLocked !== true) {
          $cache->unlock($obj);
        }

        return $obj;

      }

      // if the item is not found, unlock anyways.
      if ($getLocked !== true) {
        $cache->unlock($obj);
      }

      error_log('getByid return null');
      return null;

    } catch (Exception $e) {
      error_log(
        'caughtException e='.$e->getMessage().' eClass='.get_class($e),
      );
      throw $e;
    }
  }

  // As this can return a result set this doesn't let you lock all the tiems within
  // a result set as the number of edge cases introduced by that logic is not wanted.
  public function get<TModelClass as PgRowInterface>(
    classname<TModelClass> $model,
    ?WhereClause $where = null,
  ): PgResultSet<PgRowInterface> {

    try {

      $pgModel = $this->pgModel();

      if ($where == null) {
        $where = new WhereClause();
      }

      // TODO: Fetch us a cached version of the dataset.
      //$cachedResultSet = $cache->get($where);
      // if ( $cachedResultSet instanceof ResultSet ) {
      //   return $cachedResultSet;
      // }

      // $cache->lock($where);

      $tobj = $pgModel->data()->createRowObjectFromClassName($model);
      $sql = $this->createSql($tobj, $where);

      // 4) Get a database handle.
      $dbh = $pgModel->db()->getReadDatabase();

      // 5) Run the query against the database.
      error_log('JEO query='.$sql);
      $sth = $dbh->query($sql);

      $pgModel->stats()->incrementSqlSelects();

      $wasSuccessful = $sth->wasSuccessful();
      $rowCount = $sth->getNumRows();

      error_log(
        'JEO query wasSuccessful='.
        var_export($wasSuccessful, true).
        ' rowCount='.
        var_export($rowCount, true),
      );

      if ($wasSuccessful == true && $rowCount > 0) {

        $data = new PgResultSet($model);

        while ($sth->hasMore() === true) {

          $sth->next();

          $rawRow = $sth->fetchMap();

          // 1) grab a copy of our object to work with.
          $obj = $pgModel->data()->createRowObjectFromClassName($model);

          error_log('JEO attemptToHydrateRow='.json_encode($rawRow));
          $pgModel->data()->hydrateDataToRowObject($obj, $rawRow);

          $data->add($obj);

        }

        // $cache->set($obj);

        // explicitly unlock
        //  $cache->unlock($where);
        return $data;

      }

      // if the item is not found, unlock anyways.
      //   $cache->unlock($where);
      //
      $data = new PgResultSet($model);
      return $data;

    } catch (Exception $e) {
      throw $e;
    }

  }

}